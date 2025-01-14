#pragma once
#include <string>
#include <chrono>
#include "jsonhelper.hh"
#include "nlohmann/json.hpp"
#include "sqlwriter.hh"
#include <mutex>
#include "httplib.h"

struct DTime
{
  void start()
  {
    d_start =   std::chrono::steady_clock::now();
  }
  uint32_t lapUsec()
  {
    auto usec = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now()- d_start).count();
    start();
    return usec;
  }

  std::chrono::time_point<std::chrono::steady_clock> d_start;
};

struct LockedSqw
{
  LockedSqw(const LockedSqw&) = delete;
  LockedSqw(SQLiteWriter& sqw_, std::mutex& sqwlock_) : sqw(sqw_), sqwlock(sqwlock_){}
  
  SQLiteWriter& sqw;
  std::mutex& sqwlock;
  auto query(const std::string& query, const std::initializer_list<SQLiteWriter::var_t>& values ={})
  {
    std::lock_guard<std::mutex> l(sqwlock);
    return sqw.queryT(query, values);
  }

  void queryJ(httplib::Response &res, const std::string& q, const std::initializer_list<SQLiteWriter::var_t>& values={}) 
  {
    auto result = query(q, values);
    res.set_content(packResultsJsonStr(result), "application/json");
  }

  auto queryJRet(const std::string& q, const std::initializer_list<SQLiteWriter::var_t>& values={}) 
  {
    auto result = query(q, values);
    return packResultsJson(result);
  }
  
  void addValue(const std::initializer_list<std::pair<const char*, SQLiteWriter::var_t>>& values, const std::string& table="data")
  {
    std::lock_guard<std::mutex> l(sqwlock);
    sqw.addValue(values, table);
  }
  void addValue(const std::vector<std::pair<const char*, SQLiteWriter::var_t>>& values, const std::string& table="data")
  {
    std::lock_guard<std::mutex> l(sqwlock);
    sqw.addValue(values, table);
  }

};

// we add the / to prefix for you
std::string makePathForId(const std::string& id, const std::string& prefix="docs", const std::string& suffix="", bool makepath=false);
bool isPresentNonEmpty(const std::string& id, const std::string& prefix="docs", const std::string& suffix="");
bool isPresentRightSize(const std::string& id, int64_t size, const std::string& prefix="docs");
bool cacheIsNewer(const std::string& id, const std::string& cacheprefix, const std::string& suffix, const std::string& docprefix);
bool isPDF(const std::string& fname);
bool isDocx(const std::string& fname);
bool isDoc(const std::string& fname);
bool isRtf(const std::string& fname);
bool isXML(const std::string& fname);
uint64_t getRandom64();
bool endsWith(const std::string& str, const std::string& suffix);
