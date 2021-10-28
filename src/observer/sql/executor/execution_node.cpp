/* Copyright (c) 2021 Xie Meiyi(xiemeiyi@hust.edu.cn) and OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

//
// Created by Wangyunlai on 2021/5/14.
//

#include "sql/executor/execution_node.h"

#include <sstream>

#include "common/log/log.h"
#include "storage/common/table.h"

using FLOAT = std::numeric_limits<float>;
using INT = std::numeric_limits<int>;

SelectExeNode::SelectExeNode() : table_(nullptr) {
}

SelectExeNode::~SelectExeNode() {
  for (DefaultConditionFilter * &filter : condition_filters_) {
    delete filter;
  }
  condition_filters_.clear();
}

RC SelectExeNode::init(Trx *trx, Table *table, TupleSchema &&tuple_schema, std::vector<DefaultConditionFilter *> &&condition_filters) {
  trx_ = trx;
  table_ = table;
  tuple_schema_ = tuple_schema;
  condition_filters_ = std::move(condition_filters);
  return RC::SUCCESS;
}

void record_reader(const char *data, void *context) {
  TupleRecordConverter *converter = (TupleRecordConverter *)context;
  converter->add_record(data);
}
RC SelectExeNode::execute(TupleSet &tuple_set) {
  CompositeConditionFilter condition_filter;
  condition_filter.init((const ConditionFilter **)condition_filters_.data(), condition_filters_.size());

  tuple_set.clear();
  tuple_set.set_schema(tuple_schema_);
  TupleRecordConverter converter(table_, tuple_set);
  return table_->scan_record(trx_, &condition_filter, -1, (void *)&converter, record_reader);
}

AggregateExeNode::~AggregateExeNode() noexcept {

}

RC AggregateExeNode::init(Trx *trx, TupleSchema &tupleSchema, TupleSchema &group,
        std::vector<AggType>& aggTypes, ExecutionNode* child){
  trx_ = trx;
  tupleSchema_ = tupleSchema; // for group by if empty means no group by
  aggTypes_ = aggTypes;
  childNode_ = child;
  group_ = group;
  return RC::SUCCESS;
}

RC AggregateExeNode::initHash(bool isGroup, const Key& key) {
  std::vector<TupleValue*>* v;
  if (!isGroup) {
    v = &values_;
  }else {
    gvalues_.insert({key, std::vector<TupleValue*>()});
    v = &gvalues_[key];
  }
  for (size_t i = 0; i < aggTypes_.size(); i++) {
    switch (aggTypes_[i]) {
      case COUNT_AGG:
        v->emplace_back(new IntValue(0));
        break;
      case MIN_AGG:
        if (tupleSchema_.field(i).type() == FLOATS)
          v->emplace_back(new FloatValue(FLOAT::max()));
        else if (tupleSchema_.field(i).type() == INTS)
          v->emplace_back(new IntValue(INT::max()));
        else if (tupleSchema_.field(i).type() == CHARS)
          v->emplace_back(new StringValue("\xff\xff\xff\xff"));
        else if (tupleSchema_.field(i).type() == DATES)
          v->emplace_back(new DateValue(20380201));
        break;
      case MAX_AGG:
        if (tupleSchema_.field(i).type() == FLOATS)
          v->emplace_back(new FloatValue(FLOAT::min()));
        else if (tupleSchema_.field(i).type() == INTS)
          v->emplace_back(new IntValue(INT::min()));
        else if (tupleSchema_.field(i).type() == CHARS)
          v->emplace_back(new StringValue("\x00\x00\x00\x00"));
        else if (tupleSchema_.field(i).type() == DATES)
          v->emplace_back(new DateValue(19700101));
        break;
      case AVG_AGG:
        if (tupleSchema_.field(i).type() == FLOATS)
          v->emplace_back(new FloatValue(0));
        else if (tupleSchema_.field(i).type() == INTS)
          v->emplace_back(new IntValue(0));
        break;
      case NO_AGG:
        if (tupleSchema_.field(i).type() == FLOATS)
          v->emplace_back(new FloatValue(FLOAT::min()));
        else if (tupleSchema_.field(i).type() == INTS)
          v->emplace_back(new IntValue(INT::min()));
        else if (tupleSchema_.field(i).type() == CHARS)
          v->emplace_back(new StringValue("\x00\x00\x00\x00"));
        else if (tupleSchema_.field(i).type() == DATES)
          v->emplace_back(new DateValue(19700101));
        break;
    }
  }
  return RC::SUCCESS;
}

RC AggregateExeNode::execute(TupleSet &outputSet) {
  outputSet.clear();
  TupleSet tuple_set;
  tuple_set.clear();
  TupleSchema outputSchema;
  outputSchema.append(tupleSchema_);
  outputSet.set_schema(outputSchema);
  RC rc = childNode_->execute(tuple_set);
  if (rc != RC::SUCCESS) {
    return rc;
  }
  LOG_INFO("execute child node");
  std::vector<int> groups;
  std::vector<int> aggregations;
  for (int i = 0; i < group_.fields().size(); i++) {
    for (int j = 0; j < tuple_set.schema().fields().size(); j++) {
      if (tuple_set.schema().field(j).to_string() == group_.field(i).to_string()) {
        groups.push_back(j);
      }
    }
  }
  for (int i = 0; i < tupleSchema_.fields().size(); i++) {
    if (0 == strcmp(tupleSchema_.field(i).field_name(), "*") ||
        0 == strcmp(tupleSchema_.field(i).field_name(), "1")) {
      aggregations.push_back(0);
      continue;
    }
    for (int j = 0; j < tuple_set.schema().fields().size(); j++) {
      if (tuple_set.schema().field(j).to_string() == tupleSchema_.field(i).to_string()) {
        aggregations.push_back(j);
      }
    }
  }
  int n = aggTypes_.size();
  for (int i = 0; i < n; i++) {
    if (aggTypes_[i] == AVG_AGG) {
      aggTypes_.emplace_back(COUNT_AGG);
      tupleSchema_.add(INTS, tupleSchema_.field(i).table_name(), tupleSchema_.field(i).field_name());
    }
  }
  MakeKey makeKey(groups);
  int first;
  LOG_INFO("group is %d", groups.size());
  for (auto &tuple : tuple_set.tuples()) {
    first = 0;
    std::vector<TupleValue *>* v;
    auto key = makeKey(tuple);
    if (groups.empty()) {
      if (values_.empty()) {
        initHash(false, key);
        first = 1;
      }
      v = &values_;
    } else {
      if (0 == gvalues_.count(key)) {
        initHash(true, key);
        first = 1;
      }
      v = &gvalues_[key];
    }
    for (int i = 0; i < aggTypes_.size(); i++) {
      switch (aggTypes_[i]) {
        case AVG_AGG:
          v->operator[](i)->addValue(tuple.get(aggregations[i]));
          break;
        case MAX_AGG:
          if (v->operator[](i)->compare(tuple.get(aggregations[i])) == -1) {
            v->operator[](i)->setValue(tuple.get(aggregations[i]));
          }
          break;
        case MIN_AGG:
          if (v->operator[](i)->compare(tuple.get(aggregations[i])) == 1) {
            v->operator[](i)->setValue(tuple.get(aggregations[i]));
          }
          break;
        case COUNT_AGG:
          v->operator[](i)->addValue(IntValue(1));
          break;
        case NO_AGG:
          if (first == 1) {
            v->operator[](i)->setValue(tuple.get(aggregations[i]));
          }else {
            if (v->operator[](i)->compare(tuple.get(aggregations[i])) != 0) {
              return RC::SQL_SYNTAX;
            }
          }
          break;
      }
    }
  }
  LOG_INFO("execute agg value size %d", values_.size());
  if (groups.empty()) {
    int idx = n;
    for (int i = 0; i < n; i++) {
      if (aggTypes_[i] == AVG_AGG) {
        auto ptr = values_[i];
        auto num = ptr->getValue();
        values_[i] = new FloatValue(num / values_[idx]->getValue());
        delete ptr;
      }
    }
    Tuple tuple;
    for (int i = 0; i < n; i++) {
      tuple.add(values_[i]);
    }
    outputSet.add(std::move(tuple));
  }else {
    for (auto& v : gvalues_) {
      Tuple tuple;
      int idx = n;
      for (int i = 0; i < n; i++) {
        if (aggTypes_[i] == AVG_AGG) {
          auto ptr = v.second[i];
          auto num = ptr->getValue();
          v.second[i] = new FloatValue(num / v.second[idx]->getValue());
          delete ptr;
          idx++;
        }
        tuple.add(v.second[i]);
      }
      outputSet.add(std::move(tuple));
    }
  }
  return RC::SUCCESS;
}


JoinExeNode::JoinExeNode() {}

JoinExeNode::~JoinExeNode() noexcept {}

RC JoinExeNode::init(Trx *trx, std::vector<ExecutionNode*>&& childNodes,
        std::vector<Condition> &&conditions) {
  trx_ = trx;
  conditions_ = std::move(conditions);
  childNodes_ = std::move(childNodes);
  return RC::SUCCESS;
}

RC JoinExeNode::execute(TupleSet &tuple_set) {
  TupleSet leftTupleSet, rightTupleSet;
  childNodes_[0]->execute(leftTupleSet);
  childNodes_[1]->execute(rightTupleSet);
  TupleSchema left_schema = childNodes_[0]->getOutputSchema();
  TupleSchema right_schema = childNodes_[1]->getOutputSchema();
  std::stringstream ss;

  left_schema.print(ss);
  right_schema.print(ss);
  LOG_INFO("input %s %d %d", ss.str().c_str(), leftTupleSet.size(), rightTupleSet.size());
  for (auto& field : left_schema.fields()){
    tupleSchema_.add_if_not_exists(field.type(), field.table_name(), field.field_name());
  }

  for (auto& field : right_schema.fields()){
    tupleSchema_.add_if_not_exists(field.type(), field.table_name(), field.field_name());
  }
  ss.clear();
  tupleSchema_.print(ss);

  tuple_set.clear();
  tuple_set.set_schema(tupleSchema_);

  std::vector<int> idx_left;
  std::vector<int> idx_right;
  for (int i = 0; i < conditions_.size(); i++) {

    for (int j = 0; j < left_schema.fields().size(); j++) {
      if (0 == strcmp(left_schema.field(j).table_name(), conditions_[i].left_attr.relation_name) &&
          0 == strcmp(left_schema.field(j).field_name(), conditions_[i].left_attr.attribute_name)) {
        for (int k = 0; k < right_schema.fields().size(); k++) {
          if (0 == strcmp(right_schema.field(k).table_name(), conditions_[i].right_attr.relation_name) &&
              0 == strcmp(right_schema.field(k).field_name(), conditions_[i].right_attr.attribute_name)) {
           idx_left.push_back(j); idx_right.push_back(k);
          }
        }
      }

      if (0 == strcmp(left_schema.field(j).table_name(), conditions_[i].right_attr.relation_name) &&
          0 == strcmp(left_schema.field(j).field_name(), conditions_[i].right_attr.attribute_name)) {
        for (int k = 0; k < right_schema.fields().size(); k++) {
          if (0 == strcmp(right_schema.field(k).table_name(), conditions_[i].left_attr.relation_name) &&
              0 == strcmp(right_schema.field(k).field_name(), conditions_[i].left_attr.attribute_name)) {
            idx_left.push_back(j); idx_right.push_back(k);
          }
        }
      }
    }

    LOG_INFO("l_idx r_idx %d %d", idx_left.back(), idx_right.back());
  }
  for (int i = 0; i < leftTupleSet.size(); i++) {
    for (int j = 0; j < rightTupleSet.size(); j++) {
      if (cmp(idx_left, idx_right, leftTupleSet.get(i), rightTupleSet.get(j))) {
        Tuple output;

        for (auto& field : leftTupleSet.get(i).values()) {
          output.add(field);
        }
        for (auto& field : rightTupleSet.get(j).values()) {
          output.add(field);
        }
        tuple_set.add(std::move(output));
      }
    }
  }
  return RC::SUCCESS;
}

bool JoinExeNode::cmp(std::vector<int>& idx_left, std::vector<int>& idx_right,
                      const Tuple& leftTuple, const Tuple& rightTuple) const {
  bool ans = true;
  for (int k = 0; k < conditions_.size(); k++) {
    switch (conditions_[k].comp) {
      case EQUAL_TO:
        ans &= (leftTuple.get(idx_left[k]).compare(rightTuple.get(idx_right[k])) == 0);
        break;
      case LESS_EQUAL:
        ans &= (leftTuple.get(idx_left[k]).compare(rightTuple.get(idx_right[k])) <= 0);
        break;
      case NOT_EQUAL:
        ans &= (leftTuple.get(idx_left[k]).compare(rightTuple.get(idx_right[k])) != 0);
        break;
      case LESS_THAN:
        ans &= (leftTuple.get(idx_left[k]).compare(rightTuple.get(idx_right[k])) < 0);
        break;
      case GREAT_EQUAL:
        ans &= (leftTuple.get(idx_left[k]).compare(rightTuple.get(idx_right[k])) >= 0);
        break;
      case GREAT_THAN:
        ans &= (leftTuple.get(idx_left[k]).compare(rightTuple.get(idx_right[k])) > 0);
        break;
      case NO_OP:
        break;
    }
  }
  return ans;
}




