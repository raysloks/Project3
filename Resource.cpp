#include "Resource.h"

ThreadPool BaseResource::thread_pool(4);
std::string BaseResource::data_location = "data/";
