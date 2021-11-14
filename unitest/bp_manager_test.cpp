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
// Created by wangyunlai.wyl on 2021
//

#include "storage/default/disk_buffer_pool.h"
#include "gtest/gtest.h"

const int KeyLength = 1024;
const int BUFFER_SIZE = 10;
void generateBuffer(char *buffer, char key) {
  for (int i = 0; i < KeyLength - 1; i++) {
    buffer[i] = key;
  }
  buffer[KeyLength - 1] = 0;
}

TEST(test_bp_manager, test_bp_manager_simple_lru) {
  int POOLSIZE = 10;
  LruBPManager bp_manager(POOLSIZE);

  for (int i = 0; i < POOLSIZE; i++) {
    bp_manager.unpin(i);
    ASSERT_EQ(bp_manager.size(), i + 1);
  }

  for (int i = 0; i < POOLSIZE / 2; i++) {
    long pageID = -1;
    bp_manager.victim(&pageID);
    ASSERT_EQ(pageID, i);
  }

  for (int i = POOLSIZE / 2; i < POOLSIZE; i++) {
    bp_manager.pin(i);
  }
  ASSERT_EQ(bp_manager.size(), 0);

  long pageID = -1;
  ASSERT_EQ(bp_manager.victim(&pageID), false);
  ASSERT_EQ(pageID, -1);


  for (int i = 0; i < POOLSIZE; i++) {
    bp_manager.unpin(i);
    ASSERT_EQ(bp_manager.size(), i + 1);
  }

  bp_manager.unpin(POOLSIZE);
  ASSERT_EQ(bp_manager.victim(&pageID), true);
  ASSERT_EQ(pageID, 0);
}

TEST(test_bp_manager, test_lru_disk_buffer_Pool) {
  LruDiskBufferPool* diskBufferPool = theGlobalLruDiskBufferPool(BUFFER_SIZE);

  const char* file_name1 = "test1";
  const char* file_name2 = "test2";

  int file_id1 = -1, file_id2 = -1;
  PageNum pageNum = -1;
  BPPageHandle writePageHandle;
  BPPageHandle readPageHandle;
  diskBufferPool->create_file(file_name1);

  diskBufferPool->open_file(file_name1, &file_id1);

  ASSERT_NE(file_id1, -1);

  char *buffer = new char[KeyLength];
  std::vector<int> pageNums;
  for (int i = 1; i <= BUFFER_SIZE + 1; i++) { // 申请 11 页 超出 lru 的范围
    diskBufferPool->allocate_page(file_id1, &writePageHandle);

    generateBuffer(buffer, i);
    char* pos;
    diskBufferPool->get_data(&writePageHandle, &pos);

    memcpy(pos, buffer, KeyLength);

    diskBufferPool->mark_dirty(&writePageHandle);
    diskBufferPool->unpin_page(&writePageHandle);

    pageNum = writePageHandle.frame->page.page_num;
    pageNums.emplace_back(pageNum);

    diskBufferPool->flush_all_pages(file_id1);
  }
  // write complete

  for (int i = 0; i <= BUFFER_SIZE; i++) { // 申请 11 页 超出 lru 的范围
    diskBufferPool->get_this_page(file_id1, pageNums[i], &readPageHandle);

    generateBuffer(buffer, i + 1);
    char* pos;
    diskBufferPool->get_data(&readPageHandle, &pos);

    pageNum = readPageHandle.frame->page.page_num;
    ASSERT_EQ(pageNum, pageNums[i]);

    ASSERT_EQ(0, strcmp(pos, buffer));

    diskBufferPool->unpin_page(&readPageHandle);
  }
}

int main(int argc, char **argv) {


  // 分析gtest程序的命令行参数
  testing::InitGoogleTest(&argc, argv);

  // 调用RUN_ALL_TESTS()运行所有测试用例
  // main函数返回RUN_ALL_TESTS()的运行结果
  return RUN_ALL_TESTS();
}