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
// Created by Wangyunlai on 2021/5/7.
//

#ifndef __OBSERVER_STORAGE_COMMON_CONDITION_FILTER_H_
#define __OBSERVER_STORAGE_COMMON_CONDITION_FILTER_H_

#include <storage/trx/trx.h>

#include <vector>

#include "rc.h"
#include "sql/executor/tuple.h"
#include "sql/parser/parse.h"

struct Record;
class Table;
class ExecutionNode;

struct ConDesc {
  bool   is_attr;     // 是否属性，false 表示是值
  int    attr_length; // 如果是属性，表示属性值长度
  int    attr_offset; // 如果是属性，表示在记录中的偏移量
  bool   is_null;     // 表示属性是否为空
  void * value;       // 如果是值类型，这里记录值的数据
};

struct ReplDesc {
  int    condition_num;
  int    attr_length; // 如果是属性，表示属性值长度
  int    attr_offset; // 如果是属性，表示在记录中的偏移量
  int    leftOrRight; // 在子查询中 是替换左边还是右边 1：left -1: right
  AttrType attr_type;
};

class ConditionFilter {
public:
  virtual ~ConditionFilter();

  /**
   * Filter one record
   * @param rec
   * @return true means match condition, false means failed to match.
   */
  virtual bool filter(const Record &rec) const = 0;

  /**
   * 查询是否经过索引， 默认经过， 如果是 null 字段不经过
   */
   virtual bool notUseIndex() const = 0;
};

class DefaultConditionFilter : public ConditionFilter {
public:
  DefaultConditionFilter();
  virtual ~DefaultConditionFilter();

  RC init(const ConDesc &left, const ConDesc &right, AttrType attr_type, CompOp comp_op);
  RC init(Table &table, const Condition &condition);

  virtual bool filter(const Record &rec) const;

public:
  const ConDesc &left() const {
    return left_;
  }

  const ConDesc &right() const {
    return right_;
  }

  CompOp comp_op() const {
    return comp_op_;
  }

  bool notUseIndex() const {
    return left_.is_null || right_.is_null || comp_op_ == IS || comp_op_ == IS_NOT;
  }

private:
  ConDesc  left_;
  ConDesc  right_;
  AttrType attr_type_ = UNDEFINED;
  CompOp   comp_op_ = NO_OP;
};

class SubqueryConditionFilter : public DefaultConditionFilter{
 public:
  SubqueryConditionFilter();
  virtual ~SubqueryConditionFilter();

//  RC init(const ConDesc &left, const std::vector<Tuple>& right, AttrType attr_type, CompOp comp_op);
//  RC init(const std::vector<Tuple>& left, const ConDesc &right, AttrType attr_type, CompOp comp_op);
//  RC init(const std::vector<Tuple>& left, const std::vector<Tuple>& right,
//          AttrType attr_type, CompOp comp_op);
//  RC init(Table &table, const Condition &condition,
//          const std::vector<Tuple>& leftTuples, const std::vector<Tuple>& rightTuples);

  RC init(Table &table, const Condition &condition, const char *db, Trx* trx,
          Selects *subquery1, Selects *subquery2);

  virtual bool filter(const Record &rec) const;

  bool cmpTwoTupleSets() const;

 public:
  const ConDesc &left() const {
    return left_;
  }
  const ConDesc &right() const {
    return right_;
  }

  CompOp comp_op() const {
    return comp_op_;
  }

  bool notUseIndex() const {
    return left_.is_null || right_.is_null;
  }

 private:
  // should be used one of them

  ConDesc left_;
  ConDesc right_;
  // 用于关联子查询
  Selects *leftSelects_ = nullptr;
  Selects *rightSelects_ = nullptr;
  std::unordered_map<Selects*, ReplDesc> maps_;
  AttrType attr_type_ = UNDEFINED;
  CompOp   comp_op_ = NO_OP;

  mutable TupleSet leftSets_;
  mutable TupleSet rightSets_;

  char *db_ = nullptr;
  Trx *trx_ = nullptr;
};

class CompositeConditionFilter : public ConditionFilter {
public:
  CompositeConditionFilter() = default;
  virtual ~CompositeConditionFilter();

  RC init(const ConditionFilter *filters[], int filter_num);
  RC init(Table &table, const Condition *conditions, int condition_num);
  virtual bool filter(const Record &rec) const;

public:
  int filter_num() const {
    return filter_num_;
  }
  const ConditionFilter &filter(int index) const {
    return *filters_[index];
  }

  bool notUseIndex() const {
    for (int i = 0; i < filter_num_; i++) {
      if (filters_[i]->notUseIndex()) {
        return true;
      }
    }
    return false;
  }

private:
  RC init(const ConditionFilter *filters[], int filter_num, bool own_memory);
private:
  const ConditionFilter **      filters_ = nullptr;
  int                           filter_num_ = 0;
  bool                          memory_owner_ = false; // filters_的内存是否由自己来控制
};

#endif // __OBSERVER_STORAGE_COMMON_CONDITION_FILTER_H_