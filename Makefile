CXX = clang++
CXXFLAGS = -std=c++17 -O3 -I. -I./glm -I./rapidjson/include -I./SDL2 -I./box2d

SRCS = main.cpp SDLGame.cpp Scene.cpp Actor.cpp ComponentDB.cpp Input.cpp TextSystem.cpp AudioSystem.cpp ImageSystem.cpp Camera.cpp Rigidbody.cpp ParticleSystem.cpp

SRCS += $(wildcard box2d/*.cpp)

OBJS = $(SRCS:.cpp=.o)

TARGET = game_engine_linux

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf  -llua5.4

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
