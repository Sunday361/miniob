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
// Created by Wangyunlai on 2021/5/13.
//

#ifndef __OBSERVER_SQL_EXECUTOR_EXECUTION_NODE_H_
#define __OBSERVER_SQL_EXECUTOR_EXECUTION_NODE_H_

#include <vector>
#include <unordered_map>
#include <map>
#include <limits>
#include "storage/common/condition_filter.h"
#include "sql/executor/tuple.h"

class Table;
class Trx;

class ExecutionNode {
public:
  ExecutionNode() = default;
  virtual ~ExecutionNode() = default;

  virtual RC execute(TupleSet &tuple_set) = 0;
  virtual void setOutputSchema(TupleSchema&& tuple_schema) = 0;
  virtual TupleSchema& getOutputSchema() = 0;
};

class SelectExeNode : public ExecutionNode {
public:
  SelectExeNode();
  virtual ~SelectExeNode();

  RC init(Trx *trx, Table *table, TupleSchema && tuple_schema, std::vector<DefaultConditionFilter *> &&condition_filters);

  RC execute(TupleSet &tuple_set) override;

  void setOutputSchema(TupleSchema&& tuple_schema){ }
  TupleSchema& getOutputSchema() {return tuple_schema_;}

private:
  Trx *trx_ = nullptr;
  Table  * table_;
  TupleSchema  tuple_schema_;
  std::vector<DefaultConditionFilter *> condition_filters_;
};

struct Key {
  std::vector<TupleValue*> keys_;
  Key(std::vector<TupleValue*>& keys): keys_(keys) {}
  bool operator<(const Key& other) const {
    for (int i = 0; i < keys_.size(); i++) {
      int cmp = keys_[i]->compare(*other.keys_[i]);
      if (cmp == 0) {
        continue;
      }else if (cmp == 1) {
        return false;
      }else {
        return true;
      }
    }
    return false;
  }
};

class AggregateExeNode : public ExecutionNode {
 public:
  AggregateExeNode() {};
  virtual ~AggregateExeNode();

  RC init(Trx *trx, TupleSchema &tupleSchema, TupleSchema &group,
          std::vector<AggType>& aggTypes, ExecutionNode* child);
  RC execute(TupleSet &tuple_set) override;

  void setOutputSchema(TupleSchema&& tuple_schema){ }
  TupleSchema& getOutputSchema() {return tupleSchema_;}

  class MakeKey {
   public:
    std::vector<int> keys_;
    MakeKey(std::vector<int>& keys): keys_(keys) {}
    Key operator()(const Tuple& tuple) const {
      std::vector<TupleValue*> key;
      for (int i = 0; i < keys_.size(); i++) {
        key.push_back(tuple.get_pointer(keys_[i]).get());
      }
      return Key(key);
    }
  };

  RC initHash(bool isGroup,const Key& key);

 private:
  Trx *trx_ = nullptr;
  TupleSchema tupleSchema_;    // for agg
  TupleSchema group_;          // for group
  std::vector<AggType> aggTypes_; // agg type, one maps one in tupleSchema
  std::vector<TupleValue*> values_;
  std::map<Key, std::vector<TupleValue*>> gvalues_;
  ExecutionNode* childNode_ = nullptr;
};

class JoinExeNode : public ExecutionNode {
 public:
  JoinExeNode();
  virtual ~JoinExeNode();

  RC init(Trx *trx, std::vector<ExecutionNode*>&& childNodes,
          std::vector<Condition> &&conditions);

  RC execute(TupleSet &tuple_set) override;
  void setOutputSchema(TupleSchema&& tuple_schema){ }
  TupleSchema& getOutputSchema() {return tupleSchema_;}

  bool cmp(std::vector<int>& idx_left, std::vector<int>& idx_right,
           const Tuple& leftTuple, const Tuple& rightTuple) const ;
 private:
  Trx *trx_ = nullptr;
  Table  * table_ = nullptr;
  TupleSchema  tupleSchema_;
  std::vector<Condition> conditions_;
  std::vector<ExecutionNode*> childNodes_;
};

#endif //__OBSERVER_SQL_EXECUTOR_EXECUTION_NODE_H_
