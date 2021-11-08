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
#include <iomanip>
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
  virtual float getValue() const = 0;

  bool isNull() const {
    return isNull_;
  }

  void setNull() {
    isNull_ = true;
  }
  bool isNull_ = false;
private:
};

class IntValue : public TupleValue {
public:
  IntValue(int value) : value_(value) {
  }

  void to_string(std::ostream &os) const override {
    if (isNull_)
      os << "NULL";
    else
      os << value_;
  }

  int compare(const TupleValue &other) const override {
    if (other.isNull()) return -1;
    float ov = other.getValue();

    if ((float)value_ == ov) return 0;
    else if ((float)value_ > ov) {
      return 1;
    }else {
      return -1;
    }
  }

  void addValue(const TupleValue &other) override {
    const IntValue & int_other = (const IntValue &)other;
    value_ += int_other.value_;
    isNull_ = false;
  }

  void setValue(const TupleValue &other) {
    const IntValue & int_other = (const IntValue &)other;
    value_ = int_other.value_;
    isNull_ = false;
  }

  float getValue() const { // special for avg
    return value_ * 1.0;
  }
private:
  int value_;
};

class FloatValue : public TupleValue {
public:
  FloatValue(float value) : value_(value) {
  }

  void to_string(std::ostream &os) const override {
    if (isNull_) {
      os << "NULL";
      return;
    }

    auto s = std::to_string(value_);
    int idx = s.size() - 1;
    while(idx >= 0 && s[idx] != '.') {
      idx--;
    }
    if (idx != -1) {
      if (s.size() - 1 - idx > 2) {
        s = s.substr(0, idx + 3);
      }
      while (s.back() == '0') {
        s.pop_back();
      }
      if (s.back() == '.') {
        s.pop_back();
      }
    }
    os << s;
  }

  int compare(const TupleValue &other) const override {
    if (other.isNull()) return -1;
    float ov = other.getValue();
    if ((float)value_ == ov) return 0;
    else if ((float)value_ > ov) {
      return 1;
    }else {
      return -1;
    }
//
//    const FloatValue & float_other = (const FloatValue &)other;
//    if(value_ == float_other.value_) return 0;
//    return value_ > float_other.value_ ? 1 : -1;
  }

  void addValue(const TupleValue &other) override {
    const FloatValue & float_other = (const FloatValue &)other;
    isNull_ = false;
    value_ += float_other.value_;
  }

  void setValue(const TupleValue &other) override {
    const FloatValue & float_other = (const FloatValue &)other;
    value_ = float_other.value_;
    isNull_ = false;
  }

  float getValue() const { // special for avg
    return value_ * 1.0;
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
    if (isNull_) {
      os << "NULL";
      return;
    }
    os << value_;
  }

  int compare(const TupleValue &other) const override {
    const StringValue &string_other = (const StringValue &)other;
    if (value_ == string_other.value_) return 0;
    return value_ > string_other.value_ ? 1: -1;
  }

  void addValue(const TupleValue &other) override { // should not be used
    const StringValue & string_other = (const StringValue &)other;
    isNull_ = false;
    value_ += string_other.value_;
  }

  void setValue(const TupleValue &other) override { // should not be used
    const StringValue & string_other = (const StringValue &)other;
    value_ = string_other.value_;
    isNull_ = false;
  }

  float getValue() const { // special for avg, not used
    return 1.0;
  }

private:
  std::string value_;
};

class DateValue : public TupleValue {
 public:
  DateValue(int value) :value_(value){
  }

  void to_string(std::ostream &os) const override {
    if (isNull_) {
      os << "NULL";
      return;
    }
    std::string s = std::to_string(value_);
    os << s.substr(0, 4) << "-" << s.substr(4, 2) << "-" << s.substr(6, 2);
  }

  int compare(const TupleValue &other) const override {
    const DateValue &date_other = (const DateValue &)other;
    if (value_ == date_other.value_) return 0;
    return value_ > date_other.value_ ? 1 : -1;
  }

  void addValue(const TupleValue &other) override {}// should not be used

  void setValue(const TupleValue &other) override {
    const DateValue &date_other = (const DateValue &)other;
    value_ = date_other.value_;
    isNull_ = false;
  }

  float getValue() const { // special for avg, not used
    return 1.0;
  }
 private:
  int value_;
};


#endif //__OBSERVER_SQL_EXECUTOR_VALUE_H_
