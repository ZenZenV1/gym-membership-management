cmake_minimum_required(VERSION 3.16)
project(GymMembershipManagement LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(gym_membership
    main.cpp
    DateUtils.cpp
    Member.cpp
    MembershipPlan.cpp
    Payment.cpp
    GymManagementSystem.cpp
)
