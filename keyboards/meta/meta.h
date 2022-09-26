/**
 * @file meta.h
 */

#pragma once

#include "amk_keycode.h"

#define LAYOUT_default( \
    K601, K602, K603, K604, K605, K606, K607, K608, K609, K610, K611, K612, K613,             K614, \
  \
    K101, K102, K103, K104, K105, K106, K107, K108, K109, K110, K111, K112, K113, K114, K115, K116, \
    K201, K202, K203, K204, K205, K206, K207, K208, K209, K210, K211, K212, K213, K214, K215, K216, \
    K301, K302, K303, K304, K305, K306, K307, K308, K309, K310, K311, K312, K313, K314, K315, \
    K401,       K402, K403, K404, K405, K406, K407, K408, K409, K410, K411, K412, K413, K414, K415, \
    K501, K502, K503,                   K506,                         K511, K512, K513, K514, K515 \
) { \
    { K601, K602, K603, K604, K605, K606, K607, K608, K609, K610, K611, K612, K613,GC_NO,GC_NO, K614}, \
    { K101, K102, K103, K104, K105, K106, K107, K108, K109, K110, K111, K112, K113, K114, K115, K116}, \
    { K201, K202, K203, K204, K205, K206, K207, K208, K209, K210, K211, K212, K213, K214, K215, K216}, \
    { K301, K302, K303, K304, K305, K306, K307, K308, K309, K310, K311, K312, K315, K313, K314,GC_NO}, \
    { K401,GC_NO, K402, K403, K404, K405, K406, K407, K408, K409, K410, K411, K412, K413, K414, K415}, \
    { K501, K502, K503,GC_NO,GC_NO,GC_NO, K506,GC_NO,GC_NO,GC_NO,GC_NO, K511, K512, K513, K514, K515}  \
}
