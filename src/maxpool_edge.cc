#include "maxpool_edge.h"

MaxPoolEdge::MaxPoolEdge(const config::Edge& edge_config) :
  Edge(edge_config),
  kernel_size_(edge_config.kernel_size()),
  stride_(edge_config.stride()),
  padding_(edge_config.padding()){}

void MaxPoolEdge::SetTiedTo(Edge* e) {
  Edge::SetTiedTo(e);
  MaxPoolEdge* ee = dynamic_cast<MaxPoolEdge*> (e);
  kernel_size_ = ee->GetKernelSize();
  stride_ = ee->GetStride();
  padding_ = ee->GetPadding();
}

void MaxPoolEdge::SetImageSize(int image_size) {
  Edge::SetImageSize(image_size);
  num_modules_ = (image_size + 2 * padding_ - kernel_size_) / stride_ + 1;
}
 
string MaxPoolEdge::GetDescription() {
  stringstream ss;
  ss << name_ << " "
     << " MaxPool Kernel: " << kernel_size_ << "-" << kernel_size_ << "-"
     << num_input_channels_ << " : " << num_output_channels_
     << " Layer: " << image_size_ << "-" << image_size_ << "-"
     << num_input_channels_ << " : " << num_modules_ << "-" << num_modules_
     << "-" << num_output_channels_;
  return ss.str();
}

void MaxPoolEdge::FOV(int* size, int* sep, int* pad1, int* pad2) const {
  *size = kernel_size_ + stride_ * ((*size) - 1);
  *sep = (*sep) * stride_;
  *pad1 = (*pad1) * stride_ + padding_;
  int k = (image_size_ + 2*padding_ - kernel_size_) / stride_;
  int effective_right_pad = k * stride_ - (image_size_ + padding_ - kernel_size_);
  *pad2 = (*pad2) * stride_ + effective_right_pad;
}

void MaxPoolEdge::ComputeUp(Matrix& input, Matrix& output, bool overwrite) {
  float scale_targets = overwrite ? 0 : 1;
  Matrix::ConvMaxPool(input, output, num_input_channels_, kernel_size_,
                      padding_, stride_, num_modules_, scale_targets);
}

void MaxPoolEdge::ComputeDown(Matrix& deriv_output, Matrix& input,
                              Matrix& output, Matrix& deriv_input, bool overwrite) {
  float scale_targets = overwrite ? 0 : 1;
  Matrix::ConvMaxPoolUndo(input, deriv_output, output, deriv_input, kernel_size_,
                          padding_, stride_, num_modules_, scale_targets);
}
