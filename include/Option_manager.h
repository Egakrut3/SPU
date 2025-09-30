#ifndef OPTION_MANAGER_H
#define OPTION_MANAGER_H

#include "Common.h"

errno_t set_config(struct User_error *error_ptr, struct Config *config_ptr,
                   size_t argc, char const *const *argv);

#endif
