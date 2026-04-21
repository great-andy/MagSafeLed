# Project names
NAME			:= MagSafeLed
IDENTIFIER		:= at.GreatAndy.MagSafeLed
# Version (Tag + commit-counter, exluding hash, including dirty-flag)
GIT_VER 		:= $(shell git describe --tags --always --dirty 2>/dev/null | sed 's/-g[0-9a-f]\{7,40\}//')
VERSION 		:= $(if $(GIT_VER),$(GIT_VER),V0.0.0-unknown)
INSTALL_PATH	:= /usr/local/bin

# Compiler & flags
CXX			:= clang++
CXXFLAGS	:= -O3 -DNDEBUG -flto -fno-exceptions -fobjc-arc -Wall -std=c++17 -arch arm64
CXXFLAGS	+= -DAPP_VERSION=\"$(VERSION)\"
LDFLAGS 	:= -framework AppKit -framework IOKit

# Folders
SRC_DIR		:= src
BUILD_DIR	:= build
PAYLOAD_DIR	:= payload
SCRIPT_DIR	:= scripts

# Find sources: .cpp .mm
SRCS := $(wildcard $(SRC_DIR)/*.cpp) $(wildcard $(SRC_DIR)/*.mm)
# Generate object files: .o
OBJS := $(patsubst $(SRC_DIR)/%, $(BUILD_DIR)/%.o, $(SRCS))
TARGET := $(BUILD_DIR)/$(NAME)

# Standard target
all: $(TARGET)

# Link objects
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) $(LDFLAGS) -o $(BUILD_DIR)/$(NAME)

# Compile sources
$(BUILD_DIR)/%.cpp.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.mm.o: $(SRC_DIR)/%.mm
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Make package
pkg: all
	@rm -rf $(PAYLOAD_DIR)
	@mkdir -p $(PAYLOAD_DIR)$(INSTALL_PATH)
	@strip -x $(BUILD_DIR)/$(NAME) -o $(PAYLOAD_DIR)$(INSTALL_PATH)/$(NAME)
	@cp scripts/uninstall.sh $(PAYLOAD_DIR)$(INSTALL_PATH)/$(NAME)_uninstall
	@chmod +x $(PAYLOAD_DIR)$(INSTALL_PATH)/$(NAME)_uninstall
	@chmod +x $(SCRIPT_DIR)/preinstall $(SCRIPT_DIR)/postinstall
	@pkgbuild --root $(PAYLOAD_DIR) \
			--scripts $(SCRIPT_DIR) \
			--identifier $(IDENTIFIER) \
			--version $(VERSION) \
			--ownership recommended \
			--install-location / \
			$(NAME)_$(VERSION).pkg

clean:
	rm -rf $(BUILD_DIR) $(PAYLOAD_DIR) $(NAME)_*.pkg

.PHONY: all pkg clean
