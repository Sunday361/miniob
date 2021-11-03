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
// Created by wangyunlai.wyl on 2021/5/18.
//

#include "storage/common/index_meta.h"
#include "storage/common/field_meta.h"
#include "storage/common/table_meta.h"
#include "common/lang/string.h"
#include "common/log/log.h"
#include "rc.h"
#include "json/json.h"

const static Json::StaticString FIELD_NAME("name");
const static Json::StaticString FIELD_FIELD_NAME("field_name");
const static Json::StaticString FIELD_FIELDS_NAME("fields_name");
const static Json::StaticString FIELD_UNIQUE("isUnique");

//RC IndexMeta::init(const char *name, const FieldMeta &field, bool isUnique) {
//  if (nullptr == name || common::is_blank(name)) {
//    return RC::INVALID_ARGUMENT;
//  }
//
//  name_ = name;
//  field_ = field.name();
//  unique_ = isUnique;
//  return RC::SUCCESS;
//}
//
//void IndexMeta::to_json(Json::Value &json_value) const {
//  json_value[FIELD_NAME] = name_;
//  json_value[FIELD_FIELD_NAME] = field_;
//  json_value[FIELD_UNIQUE] = unique_;
//}
//
//RC IndexMeta::from_json(const TableMeta &table, const Json::Value &json_value, IndexMeta &index) {
//  const Json::Value &name_value = json_value[FIELD_NAME];
//  const Json::Value &field_value = json_value[FIELD_FIELD_NAME];
//  const Json::Value &unique_value = json_value[FIELD_UNIQUE];
//  if (!name_value.isString()) {
//    LOG_ERROR("Index name is not a string. json value=%s", name_value.toStyledString().c_str());
//    return RC::GENERIC_ERROR;
//  }
//
//  if (!field_value.isString()) {
//    LOG_ERROR("Field name of index [%s] is not a string. json value=%s",
//              name_value.asCString(), field_value.toStyledString().c_str());
//    return RC::GENERIC_ERROR;
//  }
//
//  if (!unique_value.isBool()) {
//    LOG_ERROR("Field name of index [%s] is not a string. json value=%s %s",
//              name_value.asCString(), field_value.toStyledString().c_str(), unique_value.toStyledString().c_str());
//    return RC::GENERIC_ERROR;
//  }
//
//  const FieldMeta *field = table.field(field_value.asCString());
//  if (nullptr == field) {
//    LOG_ERROR("Deserialize index [%s]: no such field: %s", name_value.asCString(), field_value.asCString());
//    return RC::SCHEMA_FIELD_MISSING;
//  }
//  bool unique = unique_value.asBool();
//
//  return index.init(name_value.asCString(), *field, unique);
//}
//
//const char *IndexMeta::name() const {
//  return name_.c_str();
//}
//
//const char *IndexMeta::field() const {
//  return field_.c_str();
//}
//
//bool IndexMeta::isUnique() const {
//  return unique_ == true;
//}
//
//void IndexMeta::desc(std::ostream &os) const {
//  os << "index name=" << name_
//      << ", field=" << field_ << ", isUnique=" << unique_;
//}


RC MultiIndexMeta::init(const char *name, std::vector<FieldMeta> &fields, bool isUnique) {
  name_ = std::string(name);
  for (auto& field : fields) {
    fields_.emplace_back(field.name());
  }
  unique_ = isUnique;
  return RC::SUCCESS;
}

const char * MultiIndexMeta::name() const {
  return name_.c_str();
}
const std::vector<std::string>& MultiIndexMeta::fields() const {
  return fields_;
}
const char* MultiIndexMeta::field(int idx) const {
  return fields_[idx].c_str();
}
bool MultiIndexMeta::isUnique() const { return unique_; }

void MultiIndexMeta::desc(std::ostream &os) const {}

void MultiIndexMeta::to_json(Json::Value &json_value) const {
  json_value[FIELD_NAME] = name_;
  Json::Value fields;
  for (auto& f : fields_) {
    Json::Value v;
    v[FIELD_NAME] = f;
    fields.append(std::move(v));
  }
  json_value[FIELD_FIELDS_NAME] = fields;
}

RC MultiIndexMeta::from_json(const TableMeta &table, const Json::Value &json_value, MultiIndexMeta &index) {
  const Json::Value &name_value = json_value[FIELD_NAME];
  const Json::Value &fields_value = json_value[FIELD_FIELDS_NAME];
  //const Json::Value &unique_value = json_value[FIELD_UNIQUE];
  if (!name_value.isString()) {
    LOG_ERROR("Index name is not a string. json value=%s", name_value.toStyledString().c_str());
    return RC::GENERIC_ERROR;
  }

  if (!fields_value.isArray() || fields_value.size() <= 0) {
    LOG_ERROR("Field name of index [%s] is not a string. json value=%s",
              name_value.asCString(), fields_value.toStyledString().c_str());
    return RC::GENERIC_ERROR;
  }
  int num = fields_value.size();
  std::vector<FieldMeta> fields_meta(num);
  for (int i = 0; i < num; i++) {
    FieldMeta &meta = fields_meta[i];
    const Json::Value &field_value = fields_value[i];
    auto m = table.field(field_value[FIELD_NAME].asCString());

    if (nullptr == m) {
      LOG_ERROR("Deserialize index [%s]: no such field: %s", name_value.asCString(), field_value.asCString());
      return RC::SCHEMA_FIELD_MISSING;
    }
    meta = *m;
  }
  return index.init(name_value.asCString(), fields_meta);
}
