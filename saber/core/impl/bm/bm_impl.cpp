#include "core/tensor.h"
#include "env.h"

#include "bmlib_runtime.h"
#include "bmdnn_api.h"
#include "bmlib_utils.h"

#ifdef USE_BM
const char* bmdnn_get_errorstring(bm_status_t error) {
    switch (error) {
        case BM_SUCCESS:
            return "BM API call correct";
        case BM_ERR_FAILURE:
            return "BM API fail to return";
        case BM_ERR_TIMEOUT:
            return "BM API time out";
        case BM_ERR_PARAM:
            return "BM API invalid parameter";
        case BM_ERR_NOMEM:
            return "BM API insufficient memory";
        case BM_ERR_DATA:
            return "BM API invalid data";
        case BM_ERR_BUSY:
            return "BM device is busy";
        case BM_NOT_SUPPORTED:
            return "BM unsupported operate";
    }
    return "Unknown bmdnn status";
}
#endif

namespace anakin{

namespace saber{

#ifdef USE_BM

typedef TargetWrapper<BM, __device_target> BM_API;

//TODO: check exception
//static bm_handle_t handle = get_bm_handle();
static bm_handle_t handle;

void BM_API::get_device_count(int &count) {
    BMDNN_CHECK(bm_dev_getcount(&count));
}

void BM_API::set_device(int id){
    //(bm_handle_t &handle, bool bmkernel_used, int id){
    BMDNN_CHECK(bm_dev_request(&handle, 0, id));
}

//TODO: Do we have this functionality?
int BM_API::get_device_id(){
    return 0;
}
        
void BM_API::mem_alloc(void** ptr, size_t n){
    handle = get_bm_handle();
    /* bm_device_mem_t *mem = reinterpret_cast<struct bm_mem_desc *>(*ptr); */
    bm_device_mem_t *mem = new bm_device_mem_t();
    BMDNN_CHECK(bm_malloc_device_byte(handle, mem, n));
    *ptr = mem;
}
        
void BM_API::mem_free(void* ptr){
    if(ptr != nullptr){
        handle = get_bm_handle();
        bm_free_device(handle, *(struct bm_mem_desc*)(ptr));
        delete ptr;
    }
}
        
void BM_API::mem_set(void* ptr, int value, size_t n){
    //(bm_handle_t handle, const int value, bm_device_mem_t mem){
    BMDNN_CHECK(bm_memset_device(handle, value, bm_mem_from_system(ptr)));
    //bm_device_mem_t* pmem = (struct bm_mem_desc *)(ptr);
    //BMDNN_CHECK(bm_memset_device(handle, value, *pmem));
}

//static void sync_memcpy(void* dst, int dst_id, const void* src, int src_id, \
//    size_t count, __DtoD) {};

//static void sync_memcpy(void* dst, int dst_id, const void* src, int src_id, \
//    size_t count, __HtoD) {};

void BM_API::sync_memcpy(void* dst, int dst_id, const void* src, int src_id, \
    size_t count, __DtoH) {
    handle = get_bm_handle(); 
    //auto* dev_ptr = const_cast<bm_device_mem_t *>(src);
    BMDNN_CHECK(bm_memcpy_d2s(handle, bm_mem_from_system(dst), *(bm_device_mem_t *)(src)));
    //BMDNN_CHECK(bm_memcpy_d2s(handle, bm_mem_from_system(dst), *src));
    LOG(INFO) << "End sync_memcpy process";
};

//static void sync_memcpy_p2p(void* dst, int dst_dev, const void* src, \
//    int src_dev, size_t count) {};


//! target wrapper
template struct TargetWrapper<BM, __device_target>;

//! BM Buffer
template class Buffer<BM>;

//! BM Tensor
INSTANTIATE_TENSOR(BM, AK_BM, NCHW);

template struct Env<BM>;

#endif //USE_BM

} //namespace saber

} //namespace anakin