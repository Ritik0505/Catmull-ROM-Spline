APP_NAME = app
BUILD_DIR = ./bin
CPP_FILES :=./src/*.cpp
CPP_FILES += ./src/vendors/imgui/*.cpp
C_FIlES := ./src/*.c

APP_DEFINES :=
APP_INCLUDES := -I./src/vendors/GLFW -I./src/vendors
APP_LINKERS := -L./src/vendors/GLFW/lib -lglfw3 -framework Cocoa -framework OpenGL -framework IOKit

CXXFLAGS := -std=c++17

build:
	g++ $(CPP_FILES) $(C_FIlES) -o $(BUILD_DIR)/$(APP_NAME) $(CXXFLAGS) $(APP_DEFINES) $(APP_INCLUDES) $(APP_LINKERS)

run:
	$(BUILD_DIR)/$(APP_NAME)

clean:
	rm -f $(BUILD_DIR)/$(APP_NAME)
