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

#ifndef __OBSERVER_SQL_EXECUTOR_VALUE_H_
#define __OBSERVER_SQL_EXECUTOR_VALUE_H_

#include <string.h>

#include <string>
#include <ostream>

class TupleValue {
public:
  TupleValue() = default;
  virtual ~TupleValue() = default;

  virtual void to_string(std::ostream &os) const = 0;
  virtual int compare(const TupleValue &other) const = 0;
  virtual void addValue(const TupleValue &other) = 0;
  virtual void setValue(const TupleValue &other) = 0;
private:
};

class IntValue : public TupleValue {
public:
  IntValue(int value) : value_(value) {
  }

  void to_string(std::ostream &os) const override {
    os << value_;
  }

  int compare(const TupleValue &other) const override {
    const IntValue & int_other = (const IntValue &)other;
    if (value_ == int_other.value_) return 0;
    return value_ > int_other.value_ ? 1 : -1;
  }

  void addValue(const TupleValue &other) override {
    const IntValue & int_other = (const IntValue &)other;
    value_ += int_other.value_;
  }

  void setValue(const TupleValue &other) {
    const IntValue & int_other = (const IntValue &)other;
    value_ = int_other.value_;
  }

private:
  int value_;
};

class FloatValue : public TupleValue {
public:
  FloatValue(float value) : value_(value) {
  }

  void to_string(std::ostream &os) const override {
    os << value_;
  }

  int compare(const TupleValue &other) const override {
    const FloatValue & float_other = (const FloatValue &)other;
    if(value_ == float_other.value_) return 0;
    return value_ > float_other.value_ ? 1 : -1;
  }

  void addValue(const TupleValue &other) override {
    const FloatValue & float_other = (const FloatValue &)other;
    value_ += float_other.value_;
  }

  void setValue(const TupleValue &other) override {
    const FloatValue & float_other = (const FloatValue &)other;
    value_ = float_other.value_;
  }

private:
  float value_;
};

class StringValue : public TupleValue {
public:
  StringValue(const char *value, int len) : value_(value, len){
  }
  explicit StringValue(const char *value) : value_(value) {
  }

  void to_string(std::ostream &os) const override {
    os << value_;
  }

  int compare(const TupleValue &other) const override {
    const StringValue &string_other = (const StringValue &)other;
    return strcmp(value_.c_str(), string_other.value_.c_str());
  }

  void addValue(const TupleValue &other) override { // should not be used
    const StringValue & string_other = (const StringValue &)other;
    value_ += string_other.value_;
  }

  void setValue(const TupleValue &other) override { // should not be used
    const StringValue & string_other = (const StringValue &)other;
    value_ = string_other.value_;
  }
private:
  std::string value_;
};

class DateValue : public TupleValue {
 public:
  DateValue(int year, int month, int day) :year_(year), month_(month), day_(day){
  }

  void to_string(std::ostream &os) const override {
    os << year_ << "-" << month_ << "-" << day_;
  }

  int compare(const TupleValue &other) const override {
    const DateValue &date_other = (const DateValue &)other;
    if (year_ == date_other.year_ && month_ == date_other.month_ && day_ == date_other.day_) {
      return 0;
    }
    if (year_ != date_other.year_) return year_ > date_other.year_;
    if (month_ != date_other.month_) return month_ > date_other.month_;
    return day_ > date_other.day_;
  }

  void addValue(const TupleValue &other) override {}// should not be used

  void setValue(const TupleValue &other) override {
    const DateValue &date_other = (const DateValue &)other;
    year_ = date_other.year_;
    month_ = date_other.month_;
    day_ = date_other.day_;
  }
 private:
  int year_, month_, day_;
};


#endif //__OBSERVER_SQL_EXECUTOR_VALUE_H_
