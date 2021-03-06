//#ifndef SINGA_MODEL_OPERATION_BATCHNORM_H_
//#define SINGA_MODEL_OPERATION_BATCHNORM_H_

#include <vector>
#include "singa/core/tensor.h"

#ifdef USE_CUDNN
#include <cudnn.h>
#include "../layer/cudnn_utils.h" // check_cudnn
#endif // USE_CUDNN

#ifdef USE_MKLDNN
#include <mkldnn.hpp>

// combine scale and bias into weight format recognised by mkldnn api
static inline singa::Tensor get_bn_weight_from(const singa::Tensor &s, const singa::Tensor &b) {
  singa::Tensor w(singa::Shape{s.Size(),b.Size()});
  CopyDataToFrom(&w, s,s.Size(),0,0);
  CopyDataToFrom(&w, b,b.Size(),s.Size(),0);
  return w;
}


#endif // USE_MKLDNN

namespace singa {

class BatchNormHandle {
 public:
  BatchNormHandle(const float momentum, const Tensor& input);
  ~BatchNormHandle();

  float factor;

  size_t batchsize;
  size_t channels;
  size_t height;
  size_t width;
  bool is_2d;
  //bool train = true;
#ifdef USE_MKLDNN
  mkldnn::memory::data_type dtype;
  mkldnn::memory::dims x_dims;
  mkldnn::memory::dims y_dims;
  mkldnn::memory::desc *x_md;
  mkldnn::memory::desc *dx_md;
  mkldnn::batch_normalization_forward::desc *bn_fwd_d;
  mkldnn::batch_normalization_forward::primitive_desc *bn_fwd_pd;
  float epsilon;
  mkldnn::memory::format data_memory_format;
#endif //USE_MKLDNN
};


#ifdef USE_MKLDNN

  Tensor
  CpuBatchNormForwardInference(const BatchNormHandle &bnh, const Tensor &x, const Tensor &bnScale, const Tensor &bnBias,
                               Tensor &running_mean, Tensor &running_var);

  const std::vector<Tensor>
  CpuBatchNormForwardTraining(const BatchNormHandle &bnh, const Tensor &x, const Tensor &bnScale, const Tensor &bnBias,
                              Tensor &running_mean, Tensor &running_var);

  const std::vector<Tensor> CpuBatchNormBackwardx(const BatchNormHandle &bnh,
                                                  const Tensor &y, const Tensor &dy,
                                                  const Tensor &x,
                                                  const Tensor &bnScale, const Tensor &bnBias,
                                                  const Tensor &mean, const Tensor &var);

#endif // USE_MKLDNN


#ifdef USE_CUDNN

class CudnnBatchNormHandle: public BatchNormHandle {
 public:
  CudnnBatchNormHandle(const float momentum, const Tensor& input);

  //~CudnnBatchNormHandle();

  cudnnBatchNormMode_t mode;
  cudnnTensorDescriptor_t shape_desc = nullptr;
  cudnnTensorDescriptor_t param_desc = nullptr;
};

const std::vector<Tensor> GpuBatchNormForwardTraining(const CudnnBatchNormHandle
    &cbnh, const Tensor& x, const Tensor& bnScale, const Tensor& bnBias,
    Tensor& running_mean, Tensor& running_var);

Tensor GpuBatchNormForwardInference(const CudnnBatchNormHandle &cbnh,
                                    const Tensor& x, const Tensor& bnScale, const Tensor& bnBias,
                                    const Tensor& running_mean, const Tensor& running_var);

const std::vector<Tensor> GpuBatchNormBackward(const CudnnBatchNormHandle &cbnh,
    const Tensor& dy, const Tensor& x, const Tensor& bnScale, const Tensor& mean,
    const Tensor& var);

#endif  // USE_CUDNN

}  // namespace singa
