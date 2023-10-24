# 设置编译器和编译选项
CXX = g++
CXXFLAGS = -std=c++17 -Wall -O2

# 定义目标文件夹
BUILD_DIR = build
SRC_DIR = src
HUB_LABELING_DIR = Hub-Labeling

# 定义目标文件
TARGET = debug

# 定义源文件列表
SOURCES = $(wildcard $(SRC_DIR)/*.cpp) $(wildcard $(HUB_LABELING_DIR)/*.cpp)

# 生成对象文件列表
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SOURCES))

# 头文件列表
HEADERS = $(wildcard $(SRC_DIR)/*.h) $(wildcard $(HUB_LABELING_DIR)/*.h)

# 定义头文件搜索路径
INCLUDES = -I$(SRC_DIR) -I$(HUB_LABELING_DIR)

# 默认目标
all: $(TARGET)

# 生成可执行文件
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# 生成对象文件
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/%.o: $(HUB_LABELING_DIR)/%.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# 清理生成的文件
clean:
	rm -f $(BUILD_DIR)/*.o $(TARGET)

# 声明伪目标，以防文件名与目标名冲突
.PHONY: all clean
