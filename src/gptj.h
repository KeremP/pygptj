/**
 ********************************************************************************
 * @file    gptj.cpp
 * @brief   GPT-J model taken from [ggml/examples/gpt-j/main.cpp](https://github.com/ggerganov/ggml/tree/master/examples/gpt-j/main.cpp)
 * @par     ggml is licensed under MIT Copyright (c) 2022 Georgi Gerganov,
            please see [ggml License](./GGML_LICENSE)
 ********************************************************************************
 */

#include "ggml.h"
#include "utils.h"
#include <pybind11/pybind11.h>
namespace py = pybind11;

// default hparams (GPT-J 6B)
struct gptj_hparams {
    int32_t n_vocab = 50400;
    int32_t n_ctx   = 2048;
    int32_t n_embd  = 4096;
    int32_t n_head  = 16;
    int32_t n_layer = 28;
    int32_t n_rot   = 64;
    int32_t f16     = 1;
};

struct gptj_layer {
    // normalization
    struct ggml_tensor * ln_1_g;
    struct ggml_tensor * ln_1_b;

    // attention
    struct ggml_tensor * c_attn_q_proj_w;
    struct ggml_tensor * c_attn_k_proj_w;
    struct ggml_tensor * c_attn_v_proj_w;

    struct ggml_tensor * c_attn_proj_w;

    // ff
    struct ggml_tensor * c_mlp_fc_w;
    struct ggml_tensor * c_mlp_fc_b;

    struct ggml_tensor * c_mlp_proj_w;
    struct ggml_tensor * c_mlp_proj_b;
};

struct gptj_model {
    gptj_hparams hparams;

    // normalization
    struct ggml_tensor * ln_f_g;
    struct ggml_tensor * ln_f_b;

    struct ggml_tensor * wte; // position embedding

    struct ggml_tensor * lmh_g; // language model head
    struct ggml_tensor * lmh_b; // language model bias

    std::vector<gptj_layer> layers;

    // key + value memory
    struct ggml_tensor * memory_k;
    struct ggml_tensor * memory_v;

    //
    struct ggml_context * ctx;
    std::map<std::string, struct ggml_tensor *> tensors;
};

struct gptj_context {
     struct gptj_model model;
     struct gpt_vocab vocab;
};

bool gptj_model_load(const std::string & fname,  struct gptj_model & model, struct gpt_vocab & vocab);

struct gptj_context gptj_load_model_ctx(const std::string & fname);

void gptj_free(struct gptj_model * model);

bool gptj_eval(
        const gptj_model & model,
        const int n_threads,
        const int n_past,
        const std::vector<gpt_vocab::id> & embd_inp,
              std::vector<float>         & embd_w,
              size_t                     & mem_per_token);

int gptj_generate(gpt_params params, struct gptj_model & model, struct gpt_vocab & vocab, py::function new_text_callback, py::function logits_callback);