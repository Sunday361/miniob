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
#ifndef __OBSERVER_STORAGE_COMMON_PAGE_MANAGER_H_
#define __OBSERVER_STORAGE_COMMON_PAGE_MANAGER_H_

#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>

#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unordered_map>
#include <list>
#include <vector>

#include "rc.h"

typedef int PageNum;

//
#define BP_INVALID_PAGE_NUM (-1)
#define BP_PAGE_SIZE (1 << 12)
#define BP_PAGE_DATA_SIZE (BP_PAGE_SIZE - sizeof(PageNum) - sizeof(PageNum) - sizeof(int))
#define BP_FILE_SUB_HDR_SIZE (sizeof(BPFileSubHeader))
#define BP_BUFFER_SIZE 50
#define MAX_OPEN_FILE 1024

typedef struct {
  PageNum page_num;
  PageNum nextPage_num;
  int  nextOffset;
  char data[BP_PAGE_DATA_SIZE];
} Page;
// sizeof(Page) should be equal to BP_PAGE_SIZE

typedef struct {
  PageNum page_count;
  int allocated_pages;
} BPFileSubHeader;

typedef struct {
  bool dirty;
  unsigned int pin_count;
  unsigned long acc_time;
  int file_desc;

  Page page;
} Frame;

typedef struct {
  bool open;
  Frame *frame;
} BPPageHandle;

class BPFileHandle{
public:
  BPFileHandle() {
    memset(this, 0, sizeof(*this));
  }

public:
  bool bopen;
  const char *file_name;
  int file_desc;
  Frame *hdr_frame;
  Page *hdr_page;
  char *bitmap;
  BPFileSubHeader *file_sub_header;
} ;

class BPManager {
public:
  BPManager(int size = BP_BUFFER_SIZE) {
    this->size = size;
    frame = new Frame[size];
    allocated = new bool[size];
    for (int i = 0; i < size; i++) {
      allocated[i] = false;
      frame[i].pin_count = 0;
    }
    allocated_size_ = 0;
  }

  ~BPManager() {
    delete[] frame;
    size = 0;
    frame = nullptr;
    allocated = nullptr;
  }

  Frame *alloc();

  Frame *get(int file_desc, PageNum page_num);

  Frame *getFrame() { return frame; }

  bool *getAllocated() { return allocated; }

public:
  int size;
  Frame *frame = nullptr;
  bool *allocated = nullptr;

  int allocated_size_ = 0;

  std::vector<Frame*> free_lists_;
};

class DiskBufferPool {
public:
  /**
  * ???????????????????????????????????????????????????
  */
  RC create_file(const char *file_name);

  /**
   * ??????????????????????????????????????????????????????ID
   * @return
   */
  RC open_file(const char *file_name, int *file_id);

  /**
   * ??????fileID?????????????????????
   */
  RC close_file(int file_id);

  /**
   * ????????????ID?????????????????????????????????????????????????????????????????????
   * @return
   */
  RC get_this_page(int file_id, PageNum page_num, BPPageHandle *page_handle);

  /**
   * ???????????????????????????????????????????????????????????????????????????????????????????????????
   * ?????????????????????????????????????????????????????????????????????????????????
   * ?????????????????????????????????????????????????????????????????????????????????
   */
  RC allocate_page(int file_id, BPPageHandle *page_handle);

  /**
   * ????????????????????????????????????????????????
   */
  RC get_page_num(BPPageHandle *page_handle, PageNum *page_num);

  /**
   * ??????????????????????????????????????????????????????
   */
  RC get_data(BPPageHandle *page_handle, char **data);

  /**
   * ????????????????????????pageNum?????????????????????????????????
   */
  RC dispose_page(int file_id, PageNum page_num);

  /**
   * ?????????????????????????????????????????? ?????????????????? ????????????????????????pinned page
   * @param file_handle
   * @param page_num ???????????????page_num ??????????????????
   */
  RC force_page(int file_id, PageNum page_num);

  /**
   * ?????????????????????????????????????????????????????????????????????????????????????????????
   * ????????????????????????????????????????????????????????????????????????????????????
   */
  RC mark_dirty(BPPageHandle *page_handle);

  /**
   * ?????????????????????pageHandle???????????????????????????????????????
   * ?????????GetThisPage???AllocatePage??????????????????????????????????????????
   * ?????????????????????????????????????????????????????????????????????????????????????????????
   * ??????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????
   */
  RC unpin_page(BPPageHandle *page_handle);

  /**
   * ????????????????????????
   */
  RC get_page_count(int file_id, int *page_count);

  RC flush_all_pages(int file_id);

protected:
  RC allocate_block(Frame **buf);
  RC dispose_block(Frame *buf);

  /**
   * ?????????????????????????????????????????????????????????pinned page
   * @param file_handle
   * @param page_num ???????????????page_num ??????????????????
   */
  RC force_page(BPFileHandle *file_handle, PageNum page_num);
  RC force_all_pages(BPFileHandle *file_handle);
  RC check_file_id(int file_id);
  RC check_page_num(PageNum page_num, BPFileHandle *file_handle);
  RC load_page(PageNum page_num, BPFileHandle *file_handle, Frame *frame);
  RC flush_block(Frame *frame);

private:
  BPManager bp_manager_;
  BPFileHandle *open_list_[MAX_OPEN_FILE] = {nullptr};
};

DiskBufferPool *theGlobalDiskBufferPool();

class LruBPManager {
 public:
  LruBPManager(int size = BP_BUFFER_SIZE);

  ~LruBPManager();

  bool victim(long *frameId);

  void pin(long frameId);

  void unpin(long frameId);

  int size() const { return head_->frameId_; }
 private:

  struct Node {
    long frameId_;
    struct Node * next_;
    struct Node * prev_;
    Node(): frameId_(-1), next_(this), prev_(this) {}
  };

  std::unordered_map<long, Node*> maps_;

  std::list<Node*> nodeLists_;

  Node* head_ = nullptr; // head_->frameId ?????? lru??????????????? ??????
};

class LruDiskBufferPool {
 public:
  LruDiskBufferPool(int size = BP_BUFFER_SIZE);

  ~LruDiskBufferPool();

  long getPageId(int fileId, PageNum pageNum) const {
    long pageId = fileId;
    return (pageId << 32) | pageNum;
  }

  /**
  * ???????????????????????????????????????????????????
   */
  RC create_file(const char *file_name);

  /**
   * ??????????????????????????????????????????????????????ID
   * @return
   */
  RC open_file(const char *file_name, int *file_id);

  /**
   * ??????fileID?????????????????????
   */
  RC close_file(int file_id);

  /**
   * ????????????ID?????????????????????????????????????????????????????????????????????
   * @return
   */
  RC get_this_page(int file_id, PageNum page_num, BPPageHandle *page_handle);

  /**
   * ???????????????????????????????????????????????????????????????????????????????????????????????????
   * ?????????????????????????????????????????????????????????????????????????????????
   * ?????????????????????????????????????????????????????????????????????????????????
   */
  RC allocate_page(int file_id, BPPageHandle *page_handle);

  /**
   * ????????????????????????????????????????????????
   */
  RC get_page_num(BPPageHandle *page_handle, PageNum *page_num);

  /**
   * ??????????????????????????????????????????????????????
   */
  RC get_data(BPPageHandle *page_handle, char **data);

  /**
   * ????????????????????????pageNum?????????????????????????????????
   */
  RC dispose_page(int file_id, PageNum page_num);

  /**
   * ?????????????????????????????????????????? ?????????????????? ????????????????????????pinned page
   * @param file_handle
   * @param page_num ???????????????page_num ??????????????????
   */
  RC force_page(int file_id, PageNum page_num);

  /**
   * ?????????????????????????????????????????????????????????????????????????????????????????????
   * ????????????????????????????????????????????????????????????????????????????????????
   */
  RC mark_dirty(BPPageHandle *page_handle);
  RC reset_frame(Frame *frame);

  /**
   * ?????????????????????pageHandle???????????????????????????????????????
   * ?????????GetThisPage???AllocatePage??????????????????????????????????????????
   * ?????????????????????????????????????????????????????????????????????????????????????????????
   * ??????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????
   */
  RC unpin_page(BPPageHandle *page_handle);

  /**
   * ????????????????????????
   */
  RC get_page_count(int file_id, int *page_count);

  RC flush_all_pages(int file_id);

 protected:
  RC allocate_block(Frame **buf);
  RC dispose_block(Frame *buf);

  /**
   * ?????????????????????????????????????????????????????????pinned page
   * @param file_handle
   * @param page_num ???????????????page_num ??????????????????
   */
  RC force_page(BPFileHandle *file_handle, PageNum page_num);
  RC force_all_pages(BPFileHandle *file_handle);
  RC check_file_id(int file_id);
  RC check_page_num(PageNum page_num, BPFileHandle *file_handle);
  RC load_page(PageNum page_num, BPFileHandle *file_handle, Frame *frame);
  RC flush_block(Frame *frame);

 private:
  LruBPManager *bp_manager_ = nullptr;
  BPFileHandle *open_list_[MAX_OPEN_FILE] = {nullptr};

  std::list<int> freeLists_;

  Frame *frames_ = nullptr;

  // maps {pageID, pageNum} -> frameId
  std::unordered_map<long, Frame*> pageTables_;
};

LruDiskBufferPool *theGlobalLruDiskBufferPool(int size = BP_BUFFER_SIZE);



#endif //__OBSERVER_STORAGE_COMMON_PAGE_MANAGER_H_
