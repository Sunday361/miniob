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

AggregateExeNode::AggregateExeNode(): table_(nullptr) {

}

AggregateExeNode::~AggregateExeNode() noexcept {

}

RC AggregateExeNode::init(Trx *trx, Table *table, TupleSchema &tupleSchema,
        std::vector<AggType>& aggTypes, std::vector<DefaultConditionFilter *> &&condition_filters){
  trx_ = trx;
  table_ = table;
  tupleSchema_ = tupleSchema;
  aggTypes_ = aggTypes;
  condition_filters_ = condition_filters;
  for (size_t i = 0; i < aggTypes.size(); i++) {
    switch (aggTypes[i]) {
      case COUNT_AGG:
        values_.emplace_back(new IntValue(0)); break;
      case MIN_AGG:
        if (tupleSchema.field(i).type() == FLOATS)
          values_.emplace_back(new FloatValue(FLOAT::max()));
        else if (tupleSchema.field(i).type() == INTS)
          values_.emplace_back(new IntValue(INT::max()));
        else if (tupleSchema.field(i).type() == CHARS)
          values_.emplace_back(new StringValue("\xff\xff\xff\xff"));
        break;
      case MAX_AGG:
        if (tupleSchema.field(i).type() == FLOATS)
          values_.emplace_back(new FloatValue(FLOAT::min()));
        else if (tupleSchema.field(i).type() == INTS)
          values_.emplace_back(new IntValue(INT::min()));
        else if (tupleSchema.field(i).type() == CHARS)
          values_.emplace_back(new StringValue("\x00\x00\x00\x00"));
        break;
      case AVG_AGG:
        if (tupleSchema.field(i).type() == FLOATS)
          values_.emplace_back(new FloatValue(0));
        else if (tupleSchema.field(i).type() == INTS)
          values_.emplace_back(new IntValue(0));
        break;
      default:
        return RC::INVALID_ARGUMENT;
    }
  }
  return RC::SUCCESS;
}

RC AggregateExeNode::execute(TupleSet &tuple_set) {
  tuple_set.clear();
  tuple_set.set_schema(tupleSchema_);
  CompositeConditionFilter condition_filter;
  condition_filter.init((const ConditionFilter **)condition_filters_.data(), condition_filters_.size());
  TupleRecordConverter converter(table_, tuple_set);
  RC rc = table_->scan_record(trx_, &condition_filter, -1, (void *)&converter, record_reader);
  if (rc != RC::SUCCESS) {
    return rc;
  }
  std::stringstream os;
  tuple_set.print(os);
  LOG_INFO("%s", os.str().c_str());
  for (auto& tuple : tuple_set.tuples()) {
    for (int i = 0; i < aggTypes_.size(); i++) {
      switch (aggTypes_[i]) {
        case AVG_AGG:
          values_[i]->addValue(tuple.get(i)); break;
        case MAX_AGG:
          if (values_[i]->compare(tuple.get(i)) == -1) {
            values_[i]->setValue(tuple.get(i));
          }
          break;
        case MIN_AGG:
          if (values_[i]->compare(tuple.get(i)) == 1) {
            values_[i]->setValue(tuple.get(i));
          }
          break;
        case COUNT_AGG:
          values_[i]->addValue(IntValue(1)); break;
        case NO_AGG:
          return RC::INVALID_ARGUMENT;
      }
    }
  }
  for (int i = 0; i < aggTypes_.size(); i++) {
    if (aggTypes_[i] == AVG_AGG){
      auto ptr = values_[i];
      auto num = ptr->getValue();
      values_[i] = new FloatValue(num / tuple_set.size());
      delete ptr;
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

  LOG_INFO("output %s %d", ss.str().c_str(), tuple_set.size());
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




