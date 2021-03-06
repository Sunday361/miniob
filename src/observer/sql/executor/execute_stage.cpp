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
// Created by Longda on 2021/4/13.
//

#include "execute_stage.h"

#include <functional>
#include <map>
#include <sstream>
#include <string>
#include <algorithm>

#include "common/io/io.h"
#include "common/lang/string.h"
#include "common/log/log.h"
#include "common/seda/timer_stage.h"
#include "event/execution_plan_event.h"
#include "event/session_event.h"
#include "event/sql_event.h"
#include "event/storage_event.h"
#include "session/session.h"
#include "sql/executor/tuple.h"
#include "storage/common/condition_filter.h"
#include "storage/common/table.h"
#include "storage/default/default_handler.h"
#include "storage/trx/trx.h"

using namespace common;

RC create_selection_executor(Trx *trx, const Selects &selects, const char *db, const char *table_name, SelectExeNode &select_node);

//! Constructor
ExecuteStage::ExecuteStage(const char *tag) : Stage(tag) {}

//! Destructor
ExecuteStage::~ExecuteStage() {}

//! Parse properties, instantiate a stage object
Stage *ExecuteStage::make_stage(const std::string &tag) {
  ExecuteStage *stage = new (std::nothrow) ExecuteStage(tag.c_str());
  if (stage == nullptr) {
    LOG_ERROR("new ExecuteStage failed");
    return nullptr;
  }
  stage->set_properties();
  return stage;
}

//! Set properties for this object set in stage specific properties
bool ExecuteStage::set_properties() {
  //  std::string stageNameStr(stageName);
  //  std::map<std::string, std::string> section = theGlobalProperties()->get(
  //    stageNameStr);
  //
  //  std::map<std::string, std::string>::iterator it;
  //
  //  std::string key;

  return true;
}

//! Initialize stage params and validate outputs
bool ExecuteStage::initialize() {
  LOG_TRACE("Enter");

  std::list<Stage *>::iterator stgp = next_stage_list_.begin();
  default_storage_stage_ = *(stgp++);
  mem_storage_stage_ = *(stgp++);

  LOG_TRACE("Exit");
  return true;
}

//! Cleanup after disconnection
void ExecuteStage::cleanup() {
  LOG_TRACE("Enter");

  LOG_TRACE("Exit");
}

void ExecuteStage::handle_event(StageEvent *event) {
  LOG_TRACE("Enter\n");

  handle_request(event);

  LOG_TRACE("Exit\n");
  return;
}

void ExecuteStage::callback_event(StageEvent *event, CallbackContext *context) {
  LOG_TRACE("Enter\n");

  // here finish read all data from disk or network, but do nothing here.
  ExecutionPlanEvent *exe_event = static_cast<ExecutionPlanEvent *>(event);
  SQLStageEvent *sql_event = exe_event->sql_event();
  sql_event->done_immediate();

  LOG_TRACE("Exit\n");
  return;
}

void ExecuteStage::handle_request(common::StageEvent *event) {
  ExecutionPlanEvent *exe_event = static_cast<ExecutionPlanEvent *>(event);
  SessionEvent *session_event = exe_event->sql_event()->session_event();
  Query *sql = exe_event->sqls();
  const char *current_db = session_event->get_client()->session->get_current_db().c_str();

  CompletionCallback *cb = new (std::nothrow) CompletionCallback(this, nullptr);
  if (cb == nullptr) {
    LOG_ERROR("Failed to new callback for ExecutionPlanEvent");
    exe_event->done_immediate();
    return;
  }
  exe_event->push_callback(cb);

  switch (sql->flag) {
    case SCF_SELECT: { // select
      LOG_INFO("execute select");
      select(current_db, sql, exe_event->sql_event()->session_event());
      exe_event->done_immediate();
    }
    break;
    case SCF_INSERT:
      LOG_INFO("SCF_INSERT");
    case SCF_UPDATE:
      LOG_INFO("SCF_UPDATE");
    case SCF_DELETE:
      LOG_INFO("SCF_DELETE");
    case SCF_CREATE_TABLE:
      LOG_INFO("SCF_CREATE_TABLE");
    case SCF_SHOW_TABLES:
      LOG_INFO("SCF_SHOW_TABLES");
    case SCF_DESC_TABLE:
      LOG_INFO("SCF_DESC_TABLE");
    case SCF_DROP_TABLE:
      LOG_INFO("SCF_DROP_TABLE");
    case SCF_CREATE_INDEX:
      LOG_INFO("SCF_CREATE_INDEX");
    case SCF_DROP_INDEX:
      LOG_INFO("SCF_DROP_INDEX");
    case SCF_LOAD_DATA: {
      LOG_INFO("SCF_LOAD_DATA");
      StorageEvent *storage_event = new (std::nothrow) StorageEvent(exe_event);
      if (storage_event == nullptr) {
        LOG_ERROR("Failed to new StorageEvent");
        event->done_immediate();
        return;
      }

      default_storage_stage_->handle_event(storage_event);
    }
    break;
    case SCF_SYNC: {
      RC rc = DefaultHandler::get_default().sync();
      session_event->set_response(strrc(rc));
      exe_event->done_immediate();
    }
    break;
    case SCF_BEGIN: {
      session_event->get_client()->session->set_trx_multi_operation_mode(true);
      session_event->set_response(strrc(RC::SUCCESS));
      exe_event->done_immediate();
    }
    break;
    case SCF_COMMIT: {
      Trx *trx = session_event->get_client()->session->current_trx();
      RC rc = trx->commit();
      session_event->get_client()->session->set_trx_multi_operation_mode(false);
      session_event->set_response(strrc(rc));
      exe_event->done_immediate();
    }
    break;
    case SCF_ROLLBACK: {
      Trx *trx = session_event->get_client()->session->current_trx();
      RC rc = trx->rollback();
      session_event->get_client()->session->set_trx_multi_operation_mode(false);
      session_event->set_response(strrc(rc));
      exe_event->done_immediate();
    }
    break;
    case SCF_HELP: {
      const char *response = "show tables;\n"
          "desc `table name`;\n"
          "create table `table name` (`column name` `column type`, ...);\n"
          "create index `index name` on `table` (`column`);\n"
          "insert into `table` values(`value1`,`value2`);\n"
          "update `table` set column=value [where `column`=`value`];\n"
          "delete from `table` [where `column`=`value`];\n"
          "select [ * | `columns` ] from `table`;\n";
      session_event->set_response(response);
      exe_event->done_immediate();
    }
    break;
    case SCF_EXIT: {
      // do nothing
      const char *response = "Unsupported\n";
      session_event->set_response(response);
      exe_event->done_immediate();
    }
    break;
    default: {
      exe_event->done_immediate();
      LOG_ERROR("Unsupported command=%d\n", sql->flag);
    }
  }
}

void end_trx_if_need(Session *session, Trx *trx, bool all_right) {
  if (!session->is_trx_multi_operation_mode()) {
    if (all_right) {
      trx->commit();
    } else {
      trx->rollback();
    }
  }
}

bool match_table(const Selects &selects, const char *table_name_in_condition, const char *table_name_to_match) {
  if (table_name_in_condition != nullptr) {
    return 0 == strcmp(table_name_in_condition, table_name_to_match);
  }

  return selects.relation_num == 1;
}

static RC schema_add_field(Table *table, const char *field_name, TupleSchema &schema) {
  const FieldMeta *field_meta = table->table_meta().field(field_name);
  if (nullptr == field_meta) {
    LOG_WARN("No such field. %s.%s", table->name(), field_name);
    return RC::SCHEMA_FIELD_MISSING;
  }

  schema.add_if_not_exists(field_meta->type(), table->name(), field_meta->name());
  return RC::SUCCESS;
}

static RC schema_add_field_agg(Table *table, const char *field_name, TupleSchema &schema) {
  const FieldMeta *field_meta = table->table_meta().field(field_name);
  if (nullptr == field_meta) {
    LOG_WARN("No such field. %s.%s", table->name(), field_name);
    return RC::SCHEMA_FIELD_MISSING;
  }

  schema.add(field_meta->type(), table->name(), field_meta->name());
  return RC::SUCCESS;
}

// ??????????????????????????????????????????condition?????????????????????????????????select ????????????
RC create_selection_executor(Trx *trx, const Selects &selects, const char *db, const char *table_name, SelectExeNode &select_node) {
  // ???????????????????????????Attr
  TupleSchema schema;
  Table * table = DefaultHandler::get_default().find_table(db, table_name);
  if (nullptr == table) {
    LOG_WARN("No such table [%s] in db [%s]", table_name, db);
    return RC::SCHEMA_TABLE_NOT_EXIST;
  }

  for (int i = selects.attr_num - 1; i >= 0; i--) {
    const RelAttr &attr = selects.attributes[i];
    if (nullptr == attr.relation_name || 0 == strcmp(table_name, attr.relation_name)) {
      if (0 == strcmp("*", attr.attribute_name) || 0 == strcmp("1", attr.attribute_name)) {
        // ???????????????????????????
        TupleSchema::from_table(table, schema);
        break; // ?????????????????????* ??????????????????????????????
      } else {
        // ???????????????????????????
        RC rc = schema_add_field(table, attr.attribute_name, schema);
        if (rc != RC::SUCCESS) {
          return rc;
        }
      }
    }
  }

  // ???????????????????????????????????????, ??????????????????????????????
  std::vector<DefaultConditionFilter *> condition_filters;
  for (size_t i = 0; i < selects.condition_num; i++) {
    const Condition &condition = selects.conditions[i];
    if ((condition.left_is_attr == 0 && condition.right_is_attr == 0) || // ???????????????
        (condition.left_is_attr == 1 && condition.right_is_attr == 0 && match_table(selects, condition.left_attr.relation_name, table_name)) ||  // ???????????????????????????
        (condition.left_is_attr == 0 && condition.right_is_attr == 1 && match_table(selects, condition.right_attr.relation_name, table_name)) ||  // ?????????????????????????????????
        (condition.left_is_attr == 1 && condition.right_is_attr == 1 &&
            match_table(selects, condition.left_attr.relation_name, table_name) && match_table(selects, condition.right_attr.relation_name, table_name)) // ?????????????????????????????????????????????
        ) {
      DefaultConditionFilter *condition_filter = new DefaultConditionFilter();
      RC rc = condition_filter->init(*table, condition);
      if (rc != RC::SUCCESS) {
        LOG_INFO("create condition failed");
        delete condition_filter;
        for (DefaultConditionFilter * &filter : condition_filters) {
          delete filter;
        }
        return rc;
      }
      condition_filters.push_back(condition_filter);
    }

    if (condition.left_is_attr == 1 && condition.right_is_attr == 1) {
      if (match_table(selects, condition.left_attr.relation_name, table_name)) {
        RC rc = schema_add_field(table, condition.left_attr.attribute_name, schema);
        if (rc != RC::SUCCESS) {
          return rc;
        }
      }else if (match_table(selects, condition.right_attr.relation_name, table_name)) {
        RC rc = schema_add_field(table, condition.right_attr.attribute_name, schema);
        if (rc != RC::SUCCESS) {
          return rc;
        }
      }
    }

    if (condition.right_is_attr >= 2 || condition.left_is_attr >= 2) { // ???????????????
      RC rc = RC::SUCCESS;
      int leftIdx = condition.left_is_attr >= 2 ? condition.left_is_attr - 2 : -1;
      int rightIdx = condition.right_is_attr >= 2 ? condition.right_is_attr - 2 : -1;
      int isLeftRel = 0, isRightRel = 0;
      Selects *ls = nullptr, *rs = nullptr;
      if (leftIdx != -1) {
        ls = selects.subquery[leftIdx];
      }
      if (rightIdx != -1){
        rs = selects.subquery[rightIdx];
      }

      auto *condition_filter = new SubqueryConditionFilter();
      rc = condition_filter->init(*table, condition, db, trx, ls, rs);
      if (rc != RC::SUCCESS) {
        LOG_INFO("create condition failed");
        for (DefaultConditionFilter * &filter : condition_filters) {
          delete filter;
        }
        return rc;
      }
      condition_filters.push_back(condition_filter);
    }
  }
  return select_node.init(trx, table, std::move(schema), std::move(condition_filters));
}

ExecutionNode* buildJoinExeNode(Trx* trx, std::list<ExecutionNode *>& select_nodes,
                                std::list<Condition>& conditions, std::vector<std::string>& table_names) {
  std::map<std::string, bool> tables;
  for (auto& name : table_names) {
    tables.insert({name, false});
  }
  tables[table_names[0]] = true;
  ExecutionNode *n1, *n2;
  int len = select_nodes.size();
  for (int i = 1; i < len; i++) {
    n1 = select_nodes.front(); select_nodes.pop_front();
    n2 = select_nodes.front(); select_nodes.pop_front();
    std::vector<Condition> join_conditions;
    while (!conditions.empty()) {
      auto& c = conditions.front();
      if ((tables[std::string(c.left_attr.relation_name)] && std::string(c.right_attr.relation_name) == table_names[i]) ||
          (tables[std::string(c.right_attr.relation_name)] && std::string(c.left_attr.relation_name) == table_names[i])) {
        join_conditions.emplace_back(conditions.front());
        conditions.pop_front();
      } else {
        break;
      }
    }
    tables[table_names[i]] = true;
    JoinExeNode* n = new JoinExeNode();
    n->init(trx, {n1, n2}, std::move(join_conditions));

    select_nodes.push_front(n);
  }
  return select_nodes.back();
}

std::string getAggAttribute(const char *rel, const char *attr, AggType type) {
  std::string os = "";
  switch (type) {
    case COUNT_AGG: os += "count("; break;
    case AVG_AGG: os += "avg("; break;
    case MIN_AGG: os += "min("; break;
    case MAX_AGG: os += "max("; break;
    case NO_AGG: os += ""; break;
  }
  if (rel != nullptr) {
    os += std::string(rel);
    os += ".";
  }
  os += std::string(attr);
  os += ")";
  return os;
}

std::string getOutputHead(const Selects &selects) {
  int relation_num = selects.relation_num;
  std::stringstream os;
  if (relation_num == 1) { // no relation_name
    for (int i = selects.attr_num - 1; i >= 0; i--) {
      if (selects.attributes[i].aggType != NO_AGG) {
        os << getAggAttribute(nullptr, selects.attributes[i].attribute_name, selects.attributes[i].aggType);
      }else {
        os << std::string(selects.attributes[i].attribute_name);
      }
      if (i != 0) os << " | ";
    }
  }else {
    for (int i = selects.attr_num - 1; i >= 0; i--) {
      if (selects.attributes[i].aggType != NO_AGG) {
        if (0 == strcmp("*", selects.attributes[i].attribute_name) || 0 == strcmp("1", selects.attributes[i].attribute_name)) {
          os << getAggAttribute(nullptr, selects.attributes[i].attribute_name, COUNT_AGG);
        }else {
          os << getAggAttribute(selects.attributes[i].relation_name, selects.attributes[i].attribute_name, selects.attributes[i].aggType);
        }
      }else {
        os << std::string(selects.attributes[i].relation_name) << "." << std::string(selects.attributes[i].attribute_name);
      }
      if (i != 0) os << " | ";
    }
  }
  os << "\n";
  return os.str();
}
// ?????????????????? ???????????? node->execute(TupleSet)  ????????????
// ?????????????????????????????????????????????
// ?????????????????????????????? ??????????????????????????? filter??????node???????????????
RC ExecuteStage::createNode(const char *db, const Selects& selects, TupleSet& SetAns, Trx *trx) {
  RC rc = RC::SUCCESS;
  // validation aggregation and group by
  if (selects.groupby_num == 0) { // only appear in groupby
    int AggCount = 0;
    for (int i = selects.attr_num - 1; i >= 0; i--) {
      if (selects.attributes[i].aggType != NO_AGG) {
        AggCount++;
      }
    }
    LOG_INFO("agg count %d %d", AggCount, selects.attr_num);
    if (AggCount != 0 && AggCount != selects.attr_num) {
      return RC::SQL_SYNTAX;
    }
  }
  bool isAggregation = false;
  for (int i = selects.attr_num - 1; i >= 0; i--) {
    if (selects.attributes[i].aggType != NO_AGG) {
      isAggregation = true; break;
    }
  }
  LOG_INFO("validate groupbys");
  // ??????????????????????????????????????????condition?????????????????????????????????select ????????????
  std::list<ExecutionNode *> select_nodes;
  std::vector<std::string> table_names;
  for (int i = selects.relation_num - 1; i >= 0; i--) {
    const char *table_name = selects.relations[i];
    SelectExeNode *select_node = new SelectExeNode;
    rc = create_selection_executor(trx, selects, db, table_name, *select_node);
    if (rc != RC::SUCCESS) {
      delete select_node;
      for (ExecutionNode *& tmp_node: select_nodes) {
        delete tmp_node;
      }
      return rc;
    }
    select_nodes.push_back(select_node);
    table_names.emplace_back(std::string(table_name));
  }
  LOG_INFO("create select nodes");
  if (select_nodes.empty()) {
    LOG_ERROR("No table given");
    return RC::SQL_SYNTAX;
  }
  ExecutionNode *node;
  if (select_nodes.size() == 1) {
    node = (SelectExeNode*)select_nodes.back();
  }else {
    std::list<Condition> conditions;
    for (int i = 0; i < selects.condition_num; i++) {
      auto& condition = selects.conditions[i];
      if (condition.left_is_attr == 1 && condition.right_is_attr == 1 &&
          0 != strcmp(condition.left_attr.relation_name, condition.right_attr.relation_name)) {
        conditions.emplace_back(condition);
      }
    }

    node = (JoinExeNode*)buildJoinExeNode(trx, select_nodes, conditions, table_names);
  }
  LOG_INFO("create join nodes");

  // group by and aggregation

  if (isAggregation) {
    std::vector<int> agg(selects.attr_num);
    TupleSchema inSchema, groupSchema;
    std::vector<AggType> types;
    for (int i = selects.attr_num - 1; i >= 0; i--) {  //
      Table *table = nullptr;
      auto attr = selects.attributes[i];

      if (attr.relation_name == nullptr) {
        table = DefaultHandler::get_default().find_table(db, selects.relations[0]);
      } else {
        table = DefaultHandler::get_default().find_table(db, attr.relation_name);
      }

      if (attr.aggType == COUNT_AGG && (0 == strcmp(attr.attribute_name, "*") ||
                                        0 == strcmp(attr.attribute_name, "1"))) {
        const char *count = "count";
        inSchema.add(INTS, count, attr.attribute_name);
      } else {
        rc = schema_add_field_agg(table, attr.attribute_name, inSchema);
        if (rc != RC::SUCCESS) {
          return rc;
        }
      }

      types.push_back(selects.attributes[i].aggType);
    }

    for (int i = selects.groupby_num - 1; i >= 0; i--) {
      const Table *table;
      if (selects.groupbys[i].relation_name == nullptr && selects.relation_num != 1) {
        return RC::SQL_SYNTAX;
      } else if (selects.groupbys[i].relation_name == nullptr) {
        table = DefaultHandler::get_default().find_table(db, selects.relations[0]);
      } else {
        table = DefaultHandler::get_default().find_table(db, selects.groupbys[i].relation_name);
      }
      auto meta = table->table_meta().field(selects.groupbys[i].attribute_name);
      if (!meta) {
        return RC::SQL_SYNTAX;
      }
      groupSchema.add(meta->type(), table->name(), selects.groupbys[i].attribute_name);
    }

    AggregateExeNode* n = new AggregateExeNode();

    n->init(trx, inSchema, groupSchema, types, node);

    node = n;
  }
  TupleSet sets;
  rc = node->execute(sets);
  if (rc != RC::SUCCESS) {
    return rc;
  }
  LOG_INFO("execute over");

  TupleSet outputs;
  TupleSchema outputSchema;
  bool isInEqualOut = false;
  if (!isAggregation) {
    for (int i = selects.attr_num - 1; i >= 0; i--) {
      auto attr = selects.attributes[i];
      /* for avoid select *,id ... not in test */
      if (0 == strcmp("*", attr.attribute_name) && selects.attr_num == 1) {
        isInEqualOut = true;
        break;
      }
      for (int j = selects.relation_num - 1; j >= 0; j--) {
        const char *table_name = selects.relations[j];
        Table *table = DefaultHandler::get_default().find_table(db, table_name);
        if (nullptr == attr.relation_name || 0 == strcmp(table_name, attr.relation_name)) {
          RC rc = schema_add_field(table, attr.attribute_name, outputSchema);
          if (rc != RC::SUCCESS) {
            return rc;
          }
        }
      }
    }
  }
  LOG_INFO("set output schema over");
  if (!isInEqualOut && outputSchema.fields().empty() && !isAggregation) {
    return RC::SCHEMA_TABLE_NOT_EXIST;
  }

  if (isInEqualOut) {
    //sets.print(ss);
    SetAns = std::move(sets);
  } else if (!isAggregation){
    outputs.clear();
    outputs.set_schema(outputSchema);

    TupleSchema inputSchema = sets.schema();

    std::vector<int> ans(std::max(outputSchema.fields().size(), selects.attr_num), -1);

    for (int i = 0; i < inputSchema.fields().size(); i++) {
      for (int j = 0; j < outputSchema.fields().size(); j++) {
        if (inputSchema.field(i).to_string() == outputSchema.field(j).to_string()) {
          ans[j] = i;
        }
      }
    }
    for (auto& tuple : sets.tuples()) {
      auto outputTuple = Tuple();
      for (int i = 0; i < ans.size(); i++) {
        outputTuple.add(tuple.get_pointer(ans[i]));
      }
      outputs.add(std::move(outputTuple));
    }
    //ss << getOutputHead(selects);
    //outputs.printWithoutSchema(ss);
    SetAns = std::move(outputs);
  }else {
    //ss << getOutputHead(selects);
    //sets.printWithoutSchema(ss);
    SetAns = std::move(sets);
  }
  return RC::SUCCESS;
}

RC ExecuteStage::select(const char *db, Query *sql, SessionEvent *session_event) {
  RC rc = RC::SUCCESS;
  Session *session = session_event->get_client()->session;
  Trx *trx = session->current_trx();
  const Selects &selects = sql->sstr.selection;
  // validation aggregation and group by
  if (selects.groupby_num == 0) { // only appear in groupby
    int AggCount = 0;
    for (int i = selects.attr_num - 1; i >= 0; i--) {
      if (selects.attributes[i].aggType != NO_AGG) {
        AggCount++;
      }
    }
    LOG_INFO("agg count %d %d", AggCount, selects.attr_num);
    if (AggCount != 0 && AggCount != selects.attr_num) {
      session_event->set_response("FAILURE\n");
      return RC::SQL_SYNTAX;
    }
  }
  bool isAggregation = false;
  for (int i = selects.attr_num - 1; i >= 0; i--) {
    if (selects.attributes[i].aggType != NO_AGG) {
      isAggregation = true; break;
    }
  }
  LOG_INFO("validate groupbys");
  // ??????????????????????????????????????????condition?????????????????????????????????select ????????????
  std::list<ExecutionNode *> select_nodes;
  std::vector<std::string> table_names;
  for (int i = selects.relation_num - 1; i >= 0; i--) {
    const char *table_name = selects.relations[i];
    SelectExeNode *select_node = new SelectExeNode;
    rc = create_selection_executor(trx, selects, db, table_name, *select_node);
    if (rc != RC::SUCCESS) {
      delete select_node;
      for (ExecutionNode *& tmp_node: select_nodes) {
        delete tmp_node;
      }
      end_trx_if_need(session, trx, false);
      session_event->set_response("FAILURE\n");
      return rc;
    }
    select_nodes.push_back(select_node);
    table_names.emplace_back(std::string(table_name));
  }
  LOG_INFO("create select nodes");
  if (select_nodes.empty()) {
    LOG_ERROR("No table given");
    end_trx_if_need(session, trx, false);
    session_event->set_response("FAILURE\n");
    return RC::SQL_SYNTAX;
  }

  ExecutionNode* node;
  if (select_nodes.size() == 1) {
    node = (SelectExeNode*)select_nodes.back();
  }else {
    std::list<Condition> conditions;
    for (int i = 0; i < selects.condition_num; i++) {
      auto& condition = selects.conditions[i];
      if (condition.left_is_attr == 1 && condition.right_is_attr == 1 &&
          0 != strcmp(condition.left_attr.relation_name, condition.right_attr.relation_name)) {
        conditions.emplace_back(condition);
      }
    }

    node = (JoinExeNode*)buildJoinExeNode(trx, select_nodes, conditions, table_names);
  }
  LOG_INFO("create join nodes");

  // group by and aggregation

  if (isAggregation) {
    std::vector<int> agg(selects.attr_num);
    TupleSchema inSchema, groupSchema;
    std::vector<AggType> types;
    for (int i = selects.attr_num - 1; i >= 0; i--) {  //
      Table *table = nullptr;
      auto attr = selects.attributes[i];

      if (attr.relation_name == nullptr) {
        table = DefaultHandler::get_default().find_table(db, selects.relations[0]);
      } else {
        table = DefaultHandler::get_default().find_table(db, attr.relation_name);
      }

      if (attr.aggType == COUNT_AGG && (0 == strcmp(attr.attribute_name, "*") ||
                                                         0 == strcmp(attr.attribute_name, "1"))) {
        const char *count = "count";
        inSchema.add(INTS, count, attr.attribute_name);
      } else {
        rc = schema_add_field_agg(table, attr.attribute_name, inSchema);
        if (rc != RC::SUCCESS) {
          session_event->set_response("FAILURE\n");
          return rc;
        }
      }

      types.push_back(selects.attributes[i].aggType);
    }

    for (int i = selects.groupby_num - 1; i >= 0; i--) {
      const Table *table;
      if (selects.groupbys[i].relation_name == nullptr && selects.relation_num != 1) {
        session_event->set_response("FAILURE\n");
        return RC::SQL_SYNTAX;
      } else if (selects.groupbys[i].relation_name == nullptr) {
        table = DefaultHandler::get_default().find_table(db, selects.relations[0]);
      } else {
        table = DefaultHandler::get_default().find_table(db, selects.groupbys[i].relation_name);
      }
      auto meta = table->table_meta().field(selects.groupbys[i].attribute_name);
      if (!meta) {
        session_event->set_response("FAILURE\n");
        return RC::SQL_SYNTAX;
      }
      groupSchema.add(meta->type(), table->name(), selects.groupbys[i].attribute_name);
    }

    AggregateExeNode* n = new AggregateExeNode();

    n->init(trx, inSchema, groupSchema, types, node);

    node = n;
  }
  // order by
  if (selects.orderby_num > 0) {
    TupleSchema orderSchema;
    std::vector<int> orders;
    for (int i = selects.orderby_num - 1; i >= 0; i--) {
      auto& order_attr = selects.orderbys[i];
      const Table *table;
      if (order_attr.relation_name == nullptr && selects.relation_num != 1) {
        session_event->set_response("FAILURE\n");
        return RC::SQL_SYNTAX;
      }
      if (order_attr.relation_name == nullptr) {
        table = DefaultHandler::get_default().find_table(db, selects.relations[0]);
      }else {
        table = DefaultHandler::get_default().find_table(db, order_attr.relation_name);
      }

      auto meta = table->table_meta().field(order_attr.attribute_name);

      if (!meta) {
        session_event->set_response("FAILURE\n");
        return RC::SCHEMA_FIELD_NOT_EXIST;
      }

      orderSchema.add(meta->type(), table->name(), meta->name());
      orders.emplace_back(order_attr.aggType);
    }

    OrderExeNode* n = new OrderExeNode();

    n->init(trx, node, std::move(orderSchema), orders);

    node = n;
  }
  LOG_INFO("init ok");
  TupleSet sets;
  rc = node->execute(sets);
  if (rc != RC::SUCCESS) {
    session_event->set_response("FAILURE\n");
    return rc;
  }
  delete node;
  LOG_INFO("execute over");
  std::stringstream ss;
  TupleSet outputs;
  TupleSchema outputSchema;
  bool isInEqualOut = false;
  if (!isAggregation) {
    for (int i = selects.attr_num - 1; i >= 0; i--) {
      auto attr = selects.attributes[i];
      /* for avoid select *,id ... not in test */
      if (0 == strcmp("*", attr.attribute_name) && selects.attr_num == 1) {
        isInEqualOut = true;
        break;
      }
      for (int j = selects.relation_num - 1; j >= 0; j--) {
        const char *table_name = selects.relations[j];
        Table *table = DefaultHandler::get_default().find_table(db, table_name);
        if (nullptr == attr.relation_name || 0 == strcmp(table_name, attr.relation_name)) {
          rc = schema_add_field(table, attr.attribute_name, outputSchema);
          if (rc != RC::SUCCESS) {
            session_event->set_response("FAILURE\n");
            return rc;
          }
        }
      }
    }
  }
  LOG_INFO("set output schema over");
  if (!isInEqualOut && outputSchema.fields().empty() && !isAggregation) {
    session_event->set_response("FAILURE\n");
    return RC::SCHEMA_TABLE_NOT_EXIST;
  }

  if (isInEqualOut) {
    sets.print(ss);
  } else if (!isAggregation){
    outputs.clear();
    outputs.set_schema(outputSchema);

    TupleSchema inputSchema = sets.schema();

    std::vector<int> ans(std::max(outputSchema.fields().size(), selects.attr_num), -1);

    for (int i = 0; i < inputSchema.fields().size(); i++) {
      for (int j = 0; j < outputSchema.fields().size(); j++) {
        if (inputSchema.field(i).to_string() == outputSchema.field(j).to_string()) {
          ans[j] = i;
        }
      }
    }
    for (auto& tuple : sets.tuples()) {
      auto outputTuple = Tuple();
      for (int i = 0; i < ans.size(); i++) {
        outputTuple.add(tuple.get_pointer(ans[i]));
      }
      outputs.add(std::move(outputTuple));
    }
    ss << getOutputHead(selects);
    outputs.printWithoutSchema(ss);
  }else {
    ss << getOutputHead(selects);
    sets.printWithoutSchema(ss);
  }

  session_event->set_response(ss.str());
  end_trx_if_need(session, trx, true);
  return rc;
}









