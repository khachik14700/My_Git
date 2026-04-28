CC = g++
CFLAGS = -std=c++17 -Wall -Wextra
LDFLAGS = -lssl -lcrypto -lz

TARGET = mygit

SRCS = \
    src/main.cpp \
    src/cli/CommandParser.cpp \
    src/core/Repository.cpp \
    src/core/RepositoryPaths.cpp \
    src/core/RepositoryConfig.cpp \
    src/core/ObjectStore.cpp \
    src/infra/FileSystemUtils.cpp \
    src/infra/Hash.cpp \
    src/infra/Compression.cpp \
    src/objects/Blob.cpp \
    src/objects/ObjectHeader.cpp \
    src/objects/ObjectType.cpp \
    src/objects/ParsedObject.cpp \
    src/objects/TreeEntry.cpp \
    src/objects/Tree.cpp \
    src/ops/WriteTree.cpp

OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(TARGET)

re: fclean all

.PHONY: all clean fclean re