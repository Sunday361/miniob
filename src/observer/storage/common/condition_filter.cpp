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

#include "condition_filter.h"

#include <sql/executor/execute_stage.h>
#include <stddef.h>

#include "common/log/log.h"
#include "record_manager.h"
#include "storage/common/table.h"

using namespace common;

ConditionFilter::~ConditionFilter()
{}

DefaultConditionFilter::DefaultConditionFilter()
{
  left_.is_attr = false;
  left_.attr_length = 0;
  left_.attr_offset = 0;
  left_.value = nullptr;

  right_.is_attr = false;
  right_.attr_length = 0;
  right_.attr_offset = 0;
  right_.value = nullptr;
}
DefaultConditionFilter::~DefaultConditionFilter()
{}

RC DefaultConditionFilter::init(const ConDesc &left, const ConDesc &right, AttrType attr_type, CompOp comp_op)
{
  if (attr_type < CHARS || attr_type > NULLTYPE) {
    LOG_ERROR("Invalid condition with unsupported attribute type: %d", attr_type);
    return RC::INVALID_ARGUMENT;
  }

  if (comp_op < EQUAL_TO || comp_op >= NO_OP) {
    LOG_ERROR("Invalid condition with unsupported compare operation: %d", comp_op);
    return RC::INVALID_ARGUMENT;
  }

  left_ = left;
  right_ = right;
  attr_type_ = attr_type;
  comp_op_ = comp_op;
  return RC::SUCCESS;
}

RC DefaultConditionFilter::init(Table &table, const Condition &condition)
{
  const TableMeta &table_meta = table.table_meta();
  ConDesc left;
  ConDesc right;

  AttrType type_left = UNDEFINED;
  AttrType type_right = UNDEFINED;

  if (1 == condition.left_is_attr) {
    left.is_attr = true;
    const FieldMeta *field_left = table_meta.field(condition.left_attr.attribute_name);
    if (nullptr == field_left) {
      LOG_WARN("No such field in condition. %s.%s", table.name(), condition.left_attr.attribute_name);
      return RC::SCHEMA_FIELD_MISSING;
    }
    left.attr_length = field_left->len();
    left.attr_offset = field_left->offset();

    left.value = nullptr;
    left.is_null = false;
    type_left = field_left->type();
  } else {
    left.is_attr = false;
    left.value = condition.left_value.data;  // 校验type 或者转换类型
    type_left = condition.left_value.type;
    left.is_null = (type_left == NULLTYPE);
    left.attr_length = 0;
    left.attr_offset = 0;
  }

  if (1 == condition.right_is_attr) {
    right.is_attr = true;
    const FieldMeta *field_right = table_meta.field(condition.right_attr.attribute_name);
    if (nullptr == field_right) {
      LOG_WARN("No such field in condition. %s.%s", table.name(), condition.right_attr.attribute_name);
      return RC::SCHEMA_FIELD_MISSING;
    }
    right.attr_length = field_right->len();
    right.attr_offset = field_right->offset();
    type_right = field_right->type();
    right.is_null = false;
    right.value = nullptr;
  } else {
    right.is_attr = false;
    right.value = condition.right_value.data;
    type_right = condition.right_value.type;
    right.is_null = (type_right == NULLTYPE);
    right.attr_length = 0;
    right.attr_offset = 0;
  }
  long long v = 0x0100000000;
  if (left.is_null && !left.is_attr) {
    memcpy(left.value, &v, 5);
  }
  if (right.is_null && !right.is_attr) {
    memcpy(right.value, &v, 5);
  }
  // 校验和转换
  //  if (!field_type_compare_compatible_table[type_left][type_right]) {
  //    // 不能比较的两个字段， 要把信息传给客户端
  //    return RC::SCHEMA_FIELD_TYPE_MISMATCH;
  //  }
  // NOTE：这里没有实现不同类型的数据比较，比如整数跟浮点数之间的对比
  // 但是选手们还是要实现。这个功能在预选赛中会出现

  if (type_left != NULLTYPE && type_right != NULLTYPE && type_left != type_right) {
    LOG_INFO("SCHEMA_FIELD_TYPE_MISMATCH");
    return RC::SCHEMA_FIELD_TYPE_MISMATCH;
  }

  return init(left, right, type_left, condition.comp);
}

bool DefaultConditionFilter::filter(const Record &rec) const
{
  char *left_value = nullptr;
  char *right_value = nullptr;
  char isLeftNull = 0;
  char isRightNull = 0;

  if (left_.is_attr) {  // value
    left_value = (char *)(rec.data + left_.attr_offset);
    isLeftNull = *(char *)(rec.data + left_.attr_offset + left_.attr_length - 1);
  } else {
    left_value = (char *)left_.value;
  }

  if (right_.is_attr) {
    right_value = (char *)(rec.data + right_.attr_offset);
    isRightNull = *(char *)(rec.data + right_.attr_offset + right_.attr_length - 1);
  } else {
    right_value = (char *)right_.value;
  }
  // 判断 null 比较的情况 null == null / null ！= 非null
  // 其他情况全部返回 false
  LOG_INFO("%d %d %d %d", isRightNull , isLeftNull , left_.is_null ,right_.is_null);
  if (comp_op_ == IS) {
    if ((isLeftNull && right_.is_null) ||
        (left_.is_null && right_.is_null) ||
        (isRightNull && left_.is_null))
      return true;
  }

  if (comp_op_ == IS_NOT) {
    if (left_.is_attr && !isLeftNull && right_.is_null) {
      return true;
    }
    if (right_.is_attr && !isRightNull && left_.is_null) {
      return true;
    }
    if (!left_.is_attr && !left_.is_null && right_.is_null) {
      return true;
    }
    if (!right_.is_attr && !right_.is_null && left_.is_null) {
      return true;
    }
    if (left_.is_null && right_.is_null) {
      return false;
    }
  }

  if (isRightNull || isLeftNull || left_.is_null || right_.is_null) {
    return false;
  }

  int cmp_result = 0;
  switch (attr_type_) {
    case CHARS: {  // 字符串都是定长的，直接比较
      // 按照C字符串风格来定
      cmp_result = strcmp(left_value, right_value);
    } break;
    case INTS: {
      // 没有考虑大小端问题
      // 对int和float，要考虑字节对齐问题,有些平台下直接转换可能会跪
      int left = *(int *)left_value;
      int right = *(int *)right_value;
      cmp_result = left - right;
    } break;
    case FLOATS: {
      float left = *(float *)left_value;
      float right = *(float *)right_value;
      cmp_result = left - right == 0 ? 0 : (left - right > 0 ? 1:-1);
    } break;
    case DATES: {
      int left = *(int *)left_value;
      int right = *(int *)right_value;
      cmp_result = left - right;
    } break;
    default: {
    }
  }

  switch (comp_op_) {
    case EQUAL_TO:
      return 0 == cmp_result;
    case LESS_EQUAL:
      return cmp_result <= 0;
    case NOT_EQUAL:
      return cmp_result != 0;
    case LESS_THAN:
      return cmp_result < 0;
    case GREAT_EQUAL:
      return cmp_result >= 0;
    case GREAT_THAN:
      return cmp_result > 0;

    default:
      break;
  }

  LOG_PANIC("Never should print this.");
  return cmp_result;  // should not go here
}

CompositeConditionFilter::~CompositeConditionFilter()
{
  if (memory_owner_) {
    delete[] filters_;
    filters_ = nullptr;
  }
}

RC CompositeConditionFilter::init(const ConditionFilter *filters[], int filter_num, bool own_memory)
{
  filters_ = filters;
  filter_num_ = filter_num;
  memory_owner_ = own_memory;
  return RC::SUCCESS;
}
RC CompositeConditionFilter::init(const ConditionFilter *filters[], int filter_num)
{
  return init(filters, filter_num, false);
}

RC CompositeConditionFilter::init(Table &table, const Condition *conditions, int condition_num)
{
  if (condition_num == 0) {
    return RC::SUCCESS;
  }
  if (conditions == nullptr) {
    return RC::INVALID_ARGUMENT;
  }

  RC rc = RC::SUCCESS;
  ConditionFilter **condition_filters = new ConditionFilter *[condition_num];
  for (int i = 0; i < condition_num; i++) {
    DefaultConditionFilter *default_condition_filter = new DefaultConditionFilter();
    rc = default_condition_filter->init(table, conditions[i]);
    if (rc != RC::SUCCESS) {
      delete default_condition_filter;
      for (int j = i - 1; j >= 0; j--) {
        delete condition_filters[j];
        condition_filters[j] = nullptr;
      }
      delete[] condition_filters;
      condition_filters = nullptr;
      return rc;
    }
    condition_filters[i] = default_condition_filter;
  }
  return init((const ConditionFilter **)condition_filters, condition_num, true);
}

bool CompositeConditionFilter::filter(const Record &rec) const
{
  for (int i = 0; i < filter_num_; i++) {
    if (!filters_[i]->filter(rec)) {
      return false;
    }
  }
  return true;
}

SubqueryConditionFilter::SubqueryConditionFilter()
{
  left_.is_attr = false;
  left_.attr_length = 0;
  left_.attr_offset = 0;
  left_.value = nullptr;
  leftSets_.clear();
  leftSelects_ = nullptr;

  right_.is_attr = false;
  right_.attr_length = 0;
  right_.attr_offset = 0;
  right_.value = nullptr;
  rightSets_.clear();
  rightSelects_ = nullptr;
}

//RC SubqueryConditionFilter::init(const std::vector<Tuple>& left, const ConDesc &right,
//                                 AttrType attr_type, CompOp comp_op) {
//  if (attr_type < CHARS || attr_type > NULLTYPE) {
//    LOG_ERROR("Invalid condition with unsupported attribute type: %d", attr_type);
//    return RC::INVALID_ARGUMENT;
//  }
//
//  if (comp_op < EQUAL_TO || comp_op >= NO_OP) {
//    LOG_ERROR("Invalid condition with unsupported compare operation: %d", comp_op);
//    return RC::INVALID_ARGUMENT;
//  }
//
//  left_.is_attr = false;
//  right_ = right;
//  attr_type_ = attr_type;
//  comp_op_ = comp_op;
//  return RC::SUCCESS;
//}
//
//RC SubqueryConditionFilter::init(const ConDesc &left, const std::vector<Tuple>& right,
//                                 AttrType attr_type, CompOp comp_op) {
//  if (attr_type < CHARS || attr_type > NULLTYPE) {
//    LOG_ERROR("Invalid condition with unsupported attribute type: %d", attr_type);
//    return RC::INVALID_ARGUMENT;
//  }
//
//  if (comp_op < EQUAL_TO || comp_op >= NO_OP) {
//    LOG_ERROR("Invalid condition with unsupported compare operation: %d", comp_op);
//    return RC::INVALID_ARGUMENT;
//  }
//
//  left_ = left;
//  right_.is_attr = false;
//  attr_type_ = attr_type;
//  comp_op_ = comp_op;
//  return RC::SUCCESS;
//}
//
//RC SubqueryConditionFilter::init(const std::vector<Tuple>& left, const std::vector<Tuple>& right,
//                                 AttrType attr_type, CompOp comp_op) {
//  if (comp_op < EQUAL_TO || comp_op >= NO_OP) {
//    LOG_ERROR("Invalid condition with unsupported compare operation: %d", comp_op);
//    return RC::INVALID_ARGUMENT;
//  }
//
//  if (left.size() > 1 && right.size() > 1) {
//    return RC::INVALID_ARGUMENT;
//  }
//  right_.is_attr = false;
//  left_.is_attr = false;
//  attr_type_ = attr_type;
//  comp_op_ = comp_op;
//  return RC::SUCCESS;
//}

RC SubqueryConditionFilter::init(Table &table, const Condition &condition, const char *db, Trx* trx,
                                 Selects *subquery1, Selects *subquery2, int isRelated1, int isRelated2) {
  db_ = strdup(db);
  trx_ = trx;
  const TableMeta &table_meta = table.table_meta();
  AttrType type_left = UNDEFINED;
  AttrType type_right = UNDEFINED;

  if (1 == condition.left_is_attr) {
    left_.is_attr = true;
    const FieldMeta *field_left = table_meta.field(condition.left_attr.attribute_name);
    if (nullptr == field_left) {
      LOG_WARN("No such field in condition. %s.%s", table.name(), condition.left_attr.attribute_name);
      return RC::SCHEMA_FIELD_MISSING;
    }
    left_.attr_length = field_left->len();
    left_.attr_offset = field_left->offset();

    left_.value = nullptr;
    left_.is_null = false;
    type_left = field_left->type();
    attr_type_ = type_left;
  } else if (0 == condition.left_is_attr){
    left_.is_attr = false;
    left_.value = condition.left_value.data;  // 校验type 或者转换类型
    type_left = condition.left_value.type;
    left_.is_null = (type_left == NULLTYPE);
    left_.attr_length = 0;
    left_.attr_offset = 0;
  }

  if (1 == condition.right_is_attr) {
    right_.is_attr = true;
    const FieldMeta *field_right = table_meta.field(condition.right_attr.attribute_name);
    if (nullptr == field_right) {
      LOG_WARN("No such field in condition. %s.%s", table.name(), condition.right_attr.attribute_name);
      return RC::SCHEMA_FIELD_MISSING;
    }
    right_.attr_length = field_right->len();
    right_.attr_offset = field_right->offset();
    type_right = field_right->type();
    right_.is_null = false;
    right_.value = nullptr;
    attr_type_ = type_right;
  } else if (0 == condition.right_is_attr){
    right_.is_attr = false;
    right_.value = condition.right_value.data;
    type_right = condition.right_value.type;
    right_.is_null = (type_right == NULLTYPE);
    right_.attr_length = 0;
    right_.attr_offset = 0;
  }


  /**
   * 这里对子查询的条件做一下判断
   * 1. 子查询出来的 字段长度不可以大于1
   * 2. 比较符号 需要满足子查询为聚合函数
   * */
  int leftAttrLen = -1, rightAttrLen = -1;

  if (condition.left_is_attr >= 2) {
    leftAttrLen = subquery1->attr_num;
    if (leftAttrLen == 1 && 0 == strcmp(subquery1->attributes[0].attribute_name, "*") &&
        subquery1->attributes[0].aggType == NO_AGG) {
      return RC::INVALID_ARGUMENT;
    }
  }
  if (condition.right_is_attr >= 2) {
    rightAttrLen = subquery2->attr_num;
    if (rightAttrLen == 1 && 0 == strcmp(subquery2->attributes[0].attribute_name, "*") &&
        subquery2->attributes[0].aggType == NO_AGG) {
      return RC::INVALID_ARGUMENT;
    }
  }

  if (leftAttrLen > 1 && rightAttrLen > 1) {
    return RC::INVALID_ARGUMENT;
  }

//  if (condition.comp != IN && condition.comp != NOT_IN) { // 比较符号 只能出现在 子查询为聚合字段时
//    if (subquery1 && subquery1->attributes[0].aggType == NO_AGG) {
//      return RC::INVALID_ARGUMENT;
//    }
//    if (subquery2 && subquery2->attributes[0].aggType == NO_AGG) {
//      return RC::INVALID_ARGUMENT;
//    }
//  }
  /**
   * 这里需要判断subquery和外层有没有关联 如果有关联则需要在 filter 时更新子查询的 condition
   * 如果没有关联就将 子查询的结果缓存到 Tuples 中 之后直接根据结果作 filter
   * 有关联的话 也是替换condition 的值 然后将 子查询结果存入 Tuples 中 进行 filter
   * */

  if (subquery1 && !isRelated1) {
    ExecuteStage::createNode(db_, *subquery1, leftSets_, trx_);
  }else if (subquery1) {
    leftSelects_ = subquery1;
  }
  
  if (subquery2 && !isRelated2) {
    ExecuteStage::createNode(db_, *subquery2, rightSets_, trx_);
  }else if (subquery2) {
    rightSelects_ = subquery2;
  }
  if (condition.comp == IN || condition.comp == NOT_IN) {
    if (!leftSets_.tuples().empty() && leftSets_.tuples()[0].size() > 1) {
      return RC::INVALID_ARGUMENT;
    }
    if (!rightSets_.tuples().empty() && rightSets_.tuples()[0].size() > 1) {
      return RC::INVALID_ARGUMENT;
    }
  }
  if (condition.comp != IN && condition.comp != NOT_IN) {
    if (rightSets_.tuples().size() > 1) {
      return RC::INVALID_ARGUMENT;
    }
    if (leftSets_.tuples().size() > 1) {
      return RC::INVALID_ARGUMENT;
    }
  }
  if (leftSelects_ && isRelated1) { // 根据 table 和 condition 里的条件 判断出 应该选择 record 中的 offset len type
    for (int i = 0; i < leftSelects_->condition_num; i++) {
      const Condition &cond = leftSelects_->conditions[i];
      ReplDesc desc;
      if (cond.left_is_attr == 1 && 0 == strcmp(cond.left_attr.relation_name, table.name())) {
        auto field = table_meta.field(cond.left_attr.attribute_name);
        if (field == nullptr) {
          return RC::INVALID_ARGUMENT;
        }else {
          desc.is_attr = true;
          desc.attr_length = field->len();
          desc.attr_offset = field->offset();
          desc.leftOrRight = 1;
          desc.attr_type = field->type();
        }
      }else if (cond.right_is_attr == 1 && 0 == strcmp(cond.right_attr.relation_name, table.name())) {
        auto field = table_meta.field(cond.right_attr.attribute_name);
        if (field == nullptr) {
          return RC::INVALID_ARGUMENT;
        }else {
          desc.is_attr = true;
          desc.attr_length = field->len();
          desc.attr_offset = field->offset();
          desc.leftOrRight = -1;
          desc.attr_type = field->type();
        }
      }
      leftReplDesc_.emplace_back(std::move(desc));
    }
  }

  if (rightSelects_ && isRelated2) { // 根据 table 和 condition 里的条件 判断出 应该选择 record 中的 offset len type
    for (int i = 0; i < rightSelects_->condition_num; i++) {
      const Condition &cond = rightSelects_->conditions[i];
      ReplDesc desc;
      if (cond.left_is_attr == 1 && 0 == strcmp(cond.left_attr.relation_name, table.name())) {
        auto field = table_meta.field(cond.left_attr.attribute_name);
        if (field == nullptr) {
          return RC::INVALID_ARGUMENT;
        }else {
          desc.is_attr = true;
          desc.attr_length = field->len();
          desc.attr_offset = field->offset();
          desc.leftOrRight = 1;
          desc.attr_type = field->type();
        }
      }else if (cond.right_is_attr == 1 && 0 == strcmp(cond.right_attr.relation_name, table.name())) {
        auto field = table_meta.field(cond.right_attr.attribute_name);
        if (field == nullptr) {
          return RC::INVALID_ARGUMENT;
        }else {
          desc.is_attr = true;
          desc.attr_length = field->len();
          desc.attr_offset = field->offset();
          desc.leftOrRight = -1;
          desc.attr_type = field->type();
        }
      }
      rightReplDesc_.emplace_back(std::move(desc));
    }
  }
  if (subquery1) left_.is_attr = false;
  if (subquery2) right_.is_attr = false;
  comp_op_ = condition.comp;
  
  return RC::SUCCESS;
}

//RC SubqueryConditionFilter::init(Table &table, const Condition &condition,
//        const std::vector<Tuple>& leftTuples, const std::vector<Tuple>& rightTuples) {
//  const TableMeta &table_meta = table.table_meta();
//  ConDesc left;
//  ConDesc right;
//  AttrType type_left = UNDEFINED;
//  AttrType type_right = UNDEFINED;
//
//  if (1 == condition.left_is_attr) {
//    left.is_attr = true;
//    const FieldMeta *field_left = table_meta.field(condition.left_attr.attribute_name);
//    if (nullptr == field_left) {
//      LOG_WARN("No such field in condition. %s.%s", table.name(), condition.left_attr.attribute_name);
//      return RC::SCHEMA_FIELD_MISSING;
//    }
//    left.attr_length = field_left->len();
//    left.attr_offset = field_left->offset();
//
//    left.value = nullptr;
//    left.is_null = false;
//    type_left = field_left->type();
//  } else if (0 == condition.left_is_attr){
//    left.is_attr = false;
//    left.value = condition.left_value.data;  // 校验type 或者转换类型
//    type_left = condition.left_value.type;
//    left.is_null = (type_left == NULLTYPE);
//    left.attr_length = 0;
//    left.attr_offset = 0;
//  }
//
//  if (1 == condition.right_is_attr) {
//    right.is_attr = true;
//    const FieldMeta *field_right = table_meta.field(condition.right_attr.attribute_name);
//    if (nullptr == field_right) {
//      LOG_WARN("No such field in condition. %s.%s", table.name(), condition.right_attr.attribute_name);
//      return RC::SCHEMA_FIELD_MISSING;
//    }
//    right.attr_length = field_right->len();
//    right.attr_offset = field_right->offset();
//    type_right = field_right->type();
//    right.is_null = false;
//    right.value = nullptr;
//  } else if (0 == condition.right_is_attr){
//    right.is_attr = false;
//    right.value = condition.right_value.data;
//    type_right = condition.right_value.type;
//    right.is_null = (type_right == NULLTYPE);
//    right.attr_length = 0;
//    right.attr_offset = 0;
//  }
//
//  // 校验和转换
//  if (condition.comp == IN || condition.comp == NOT_IN) {
//    if (!leftTuples.empty() && leftTuples[0].size() > 1) {
//      return RC::INVALID_ARGUMENT;
//    }
//    if (!rightTuples.empty() && rightTuples[0].size() > 1) {
//      return RC::INVALID_ARGUMENT;
//    }
//  }
//  if (condition.comp != IN && condition.comp != NOT_IN) {
//    if (rightTuples.size() > 1) {
//      return RC::INVALID_ARGUMENT;
//    }
//    if (leftTuples.size() > 1) {
//      return RC::INVALID_ARGUMENT;
//    }
//  }
//
//  if (condition.left_is_attr == 1 && condition.right_is_attr >= 2) {
//    return init(left, rightTuples, type_left, condition.comp);
//  }else if (condition.left_is_attr >= 2 && condition.right_is_attr == 1) {
//    return init(leftTuples, right, type_right, condition.comp);
//  }else if (condition.left_is_attr >= 2 && condition.right_is_attr >= 2) {
//    return init(leftTuples, rightTuples, type_left, condition.comp);
//  }
//  LOG_PANIC("Never should print this.");
//  return RC::SUCCESS;
//}

bool SubqueryConditionFilter::cmpTwoTupleSets() const {
  bool ret = false;
  auto& l = leftSets_.tuples();
  auto& r = rightSets_.tuples();
  if (l.empty() || r.empty()) {
    if (comp_op_ != NOT_IN) {
      return false;
    }else {
      return true;
    }
  }
  switch (comp_op_) {
    case EQUAL_TO:
      ret = l[0].get(0).compare(r[0].get(0)) == 0;
      break;
    case LESS_EQUAL:
      ret = l[0].get(0).compare(r[0].get(0)) <= 0;
      break;
    case NOT_EQUAL:
      ret = l[0].get(0).compare(r[0].get(0)) != 0;
      break;
    case LESS_THAN:
      ret = l[0].get(0).compare(r[0].get(0)) < 0;
      break;
    case GREAT_EQUAL:
      ret = l[0].get(0).compare(r[0].get(0)) >= 0;
      break;
    case GREAT_THAN:
      ret = l[0].get(0).compare(r[0].get(0)) > 0;
      break;
    case IN:
      for (auto& t : r) {
        if (l[0].get(0).compare(t.get(0)) == 0) {
          return true;
        }
      }
      return false;
      break;
    case NOT_IN:
      for (auto& t : r) {
        if (l[0].get(0).compare(t.get(0)) == 0) {
          return false;
        }
      }
      return true;
    default:
      break;
  }
  return ret;
  LOG_PANIC("Never should print this.");
}

bool SubqueryConditionFilter::filter(const Record &rec) const
{
  char *left_value = nullptr;
  char *right_value = nullptr;

  if (leftSelects_) { // 有关联需要进行替换
    for (int i = 0; i < leftReplDesc_.size(); i++) {
      auto& desc = leftReplDesc_[i];
      if (desc.is_attr) {
        if (desc.leftOrRight == 1) {
          leftSelects_->conditions[i].left_is_attr = 0;
          memcpy(leftSelects_->conditions[i].left_value.data, rec.data+desc.attr_offset, desc.attr_length);
          leftSelects_->conditions[i].left_value.type = desc.attr_type;
        }else {
          leftSelects_->conditions[i].right_is_attr = 0;
          memcpy(leftSelects_->conditions[i].right_value.data, rec.data+desc.attr_offset, desc.attr_length);
          leftSelects_->conditions[i].right_value.type = desc.attr_type;
        }
      }
    }
    RC rc = ExecuteStage::createNode(db_, *leftSelects_, leftSets_, trx_);
  }
  if (rightSelects_) {
    for (int i = 0; i < rightReplDesc_.size(); i++) {
      auto& desc = rightReplDesc_[i];
      if (desc.is_attr) {
        if (desc.leftOrRight == 1) {
          rightSelects_->conditions[i].left_is_attr = 0;
          memcpy(rightSelects_->conditions[i].left_value.data, rec.data+desc.attr_offset, desc.attr_length);
          rightSelects_->conditions[i].left_value.type = desc.attr_type;
        }else {
          rightSelects_->conditions[i].right_is_attr = 0;
          memcpy(rightSelects_->conditions[i].right_value.data, rec.data+desc.attr_offset, desc.attr_length);
          rightSelects_->conditions[i].right_value.type = desc.attr_type;
        }
      }
    }
    RC rc = ExecuteStage::createNode(db_, *rightSelects_, rightSets_, trx_);
  }

  if (rightSets_.tuples().empty() && leftSets_.tuples().empty()) {
    if (comp_op_ == NOT_IN)
      return true;
    else
      return false;
  }

  if (left_.is_attr) {  // value
    left_value = (char *)(rec.data + left_.attr_offset);
  }

  if (right_.is_attr) {  // value
    right_value = (char *)(rec.data + right_.attr_offset);
  }

  TupleValue *leftTupleValue = nullptr;
  TupleValue *rightTupleValue = nullptr;
  // 判断 null 比较的情况 null == null / null ！= 非null
  // 其他情况全部返回 false
  switch (attr_type_) {
    case CHARS: {
      if (left_value) {
        leftTupleValue = new StringValue(left_value);
      }
      if (right_value) {
        rightTupleValue = new StringValue(right_value);
      }
    } break;
    case INTS: {
      if (left_value) {
        leftTupleValue = new IntValue(*(int*)left_value);
      }
      if (right_value) {
        rightTupleValue = new IntValue(*(int*)right_value);
      }
    } break;
    case FLOATS: {
      if (left_value) {
        leftTupleValue = new FloatValue(*(float *)left_value);
      }
      if (right_value) {
        rightTupleValue = new FloatValue(*(float *)right_value);
      }
    } break;
    case DATES: {
      if (left_value) {
        leftTupleValue = new DateValue(*(int*)left_value);
      }
      if (right_value) {
        rightTupleValue = new DateValue(*(int*)right_value);
      }
    } break;
    default: {
    }
  }

  if (!leftTupleValue && !rightTupleValue) {
    return cmpTwoTupleSets();
  }

  auto& rs = rightSets_.tuples();
  auto& ls = leftSets_.tuples();
  bool ret = false;
  switch (comp_op_) {
    case EQUAL_TO:
      if (leftTupleValue)
        ret = leftTupleValue->compare(rs[0].get(0)) == 0;
      else
        ret = rightTupleValue->compare(ls[0].get(0)) == 0;
      break;
    case LESS_EQUAL:
      if (leftTupleValue)
        ret = leftTupleValue->compare(rs[0].get(0)) <= 0;
      else
        ret = rightTupleValue->compare(ls[0].get(0)) >= 0;
      break;
    case NOT_EQUAL:
      if (leftTupleValue)
        ret = (leftTupleValue->compare(rs[0].get(0)) != 0);
      else
        ret = (rightTupleValue->compare(ls[0].get(0)) != 0);
      break;
    case LESS_THAN:
      if (leftTupleValue)
        ret = leftTupleValue->compare(rs[0].get(0)) < 0;
      else
        ret = rightTupleValue->compare(ls[0].get(0)) > 0;
      break;
    case GREAT_EQUAL:
      if (leftTupleValue)
        ret =  leftTupleValue->compare(rs[0].get(0)) >= 0;
      else
        ret =  rightTupleValue->compare(ls[0].get(0)) <= 0;
      break;
    case GREAT_THAN:
      if (leftTupleValue)
        ret =  leftTupleValue->compare(rs[0].get(0)) > 0;
      else
        ret =  rightTupleValue->compare(ls[0].get(0)) < 0;
      break;
    case IN:
      ret = false;
      if (leftTupleValue) {
        for (auto& r : rs) {
          if (leftTupleValue->compare(r.get(0)) == 0) {
            ret = true; break;
          }
        }
      } else {
        for (auto& l : ls) {
          if (rightTupleValue->compare(l.get(0)) == 0) {
            ret = true; break;
          }
        }
      }
      break;
    case NOT_IN:
      ret = true;
      if (leftTupleValue) {
        for (auto& r : rs) {
          if (leftTupleValue->compare(r.get(0)) == 0) {
            ret = false;
            break;
          }
        }
      } else {
        for (auto& l : ls) {
          if (rightTupleValue->compare(l.get(0)) == 0) {
            ret = false;
            break;
          }
        }
      }
      break;
    default:
      break;
  }
  if (rightTupleValue) delete rightTupleValue;
  if (leftTupleValue) delete leftTupleValue;
  return ret;
  LOG_PANIC("Never should print this.");
}
SubqueryConditionFilter::~SubqueryConditionFilter() {
  if (db_) {
    free(db_);
  }
}
