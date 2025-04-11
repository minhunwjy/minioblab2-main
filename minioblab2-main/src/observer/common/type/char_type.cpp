/* Copyright (c) 2021 OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

#include <iomanip>
#include "common/lang/comparator.h"
#include "common/lang/sstream.h"
#include "common/log/log.h"
#include "common/type/char_type.h"
#include "common/value.h"

int CharType::compare(const Value &left, const Value &right) const
{
  ASSERT(left.attr_type() == AttrType::CHARS && right.attr_type() == AttrType::CHARS, "invalid type");
  return common::compare_string(
      (void *)left.value_.pointer_value_, left.length_, (void *)right.value_.pointer_value_, right.length_);
}

RC CharType::set_value_from_str(Value &val, const string &data) const
{
  val.set_string(data.c_str());
  return RC::SUCCESS;
}

bool check_date(int y, int m, int d)
{
    static int mon[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    bool leap = (y%400==0 || (y%100 && y%4==0));
    return y > 0
        && (m > 0)&&(m <= 12)
        && (d > 0)&&(d <= ((m==2 && leap)?1:0) + mon[m]);
}

RC CharType::cast_to(const Value &val, AttrType type, Value &result) const
{
  switch (type) {
    case AttrType::DATES:
    {
      result.attr_type_ = AttrType::DATES;
      int y,m,d;
      if(sscanf(val.value_.pointer_value_, "%d-%d-%d", &y, &m, &d) != 3) {
        LOG_WARN("invalid date format: %s", val.value_.pointer_value_);
        return RC::INVALID_ARGUMENT;
      }
      bool check_ret = check_date(y,m,d);
      if(!check_ret) {
        LOG_WARN("invalid date format: %s", val.value_.pointer_value_);
        return RC::INVALID_ARGUMENT;
      }
      result.set_date(y,m,d);
    }break;
    default: return RC::UNIMPLEMENTED;
  }
  return RC::SUCCESS;
}

int CharType::cast_cost(AttrType type)
{
  if (type == AttrType::CHARS) {
    return 0;
  }
  return INT32_MAX;
}

RC CharType::to_string(const Value &val, string &result) const
{
  stringstream ss;
  ss << val.value_.pointer_value_;
  result = ss.str();
  return RC::SUCCESS;
}


int DateType::compare(const Value &left, const Value &right) const
{
//   ASSERT(left.attr_type() == AttrType::INTS, "left type is not integer");
//   ASSERT(right.attr_type() == AttrType::INTS || right.attr_type() == AttrType::FLOATS, "right type is not numeric");
  return common::compare_int((void *)&left.value_.int_value_, (void *)&right.value_.int_value_);
}

RC DateType::to_string(const Value &val, string &result) const
{
  stringstream ss;
  ss << std::setw(4) << std::setfill('0') << val.value_.int_value_/10000
     << std::setw(2) << std::setfill('0') << (val.value_.int_value_ %10000) / 10000
     << std::setw(2) << std::setfill('0') << val.value_.int_value_ %100;
  result = ss.str();
  return RC::SUCCESS;
}