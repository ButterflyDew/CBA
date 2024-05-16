CXX = g++ # C++编译器
CXXFLAGS = -std=c++17 -Wall -O2# 编译选项

TARGET = run # 目标可执行文件名

# 列出你的源文件
SOURCES = Hub-Labeling/global.cpp Hub-Labeling/static_hl.cpp Hub-Labeling/betweenness_centrality.cpp Hub-Labeling/hbll.cpp qhbll.cpp CBA.cpp core.cpp run.cpp
HEADERS = Hub-Labeling/global.h Hub-Labeling/static_hl.h Hub-Labeling/betweenness_centrality.h Hub-Labeling/hbll.h qhbll.h CBA.h core.h

# 根据需要添加更多源文件
# SOURCES += additional_file.cpp

# 生成目标的规则
$(TARGET): $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $@ $(SOURCES) -w

.PHONY: clean

clean:
	rm -f $(TARGET)
