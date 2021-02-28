
set(HEADER_LIST
        "${StockExchange_SOURCE_DIR}/include/engine/Book.h"
        "${StockExchange_SOURCE_DIR}/include/engine/Limit.h"
        "${StockExchange_SOURCE_DIR}/include/engine/MatchingEngine.h"
        "${StockExchange_SOURCE_DIR}/include/engine/Order.h"
        "${StockExchange_SOURCE_DIR}/include/engine/types.h"
        )

add_library(exchange_engine Limit.cpp MatchingEngine.cpp ${HEADER_LIST})

target_include_directories(exchange_engine PUBLIC ../include)


target_compile_features(exchange_engine PUBLIC cxx_std_17)

source_group(
        TREE "${PROJECT_SOURCE_DIR}/include"
        PREFIX "Header Files"
        FILES ${HEADER_LIST})
