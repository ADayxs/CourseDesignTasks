#pragma once

#ifndef COURSEDESIGNTASKS_EDIT_DISTANCE_C_H
#define COURSEDESIGNTASKS_EDIT_DISTANCE_C_H

#pragma once
#include <stddef.h>
#include "ast_serial.h"

size_t levenshtein_strvec(const StrVec *a, const StrVec *b);

double similarity_from_dist(size_t dist, size_t lenA, size_t lenB);

#endif //COURSEDESIGNTASKS_EDIT_DISTANCE_C_H