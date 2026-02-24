add_executable(benchmarkApp
    main.cpp
)

target_link_libraries(benchmarkApp
    PRIVATE # Link the benchmark application with the NeuronMetrics library, but do not expose it to other targets that link against benchmarkApp
        NeuronMetrics   
)