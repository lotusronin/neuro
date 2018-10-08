#include "catch.hpp"
#include <array>
#include "asttransforms.cpp"

TEST_CASE("Checking allowable implicit casts", "[canCast]") {
    TypeInfo current_t;
    TypeInfo result_t;
    SECTION("u8 implicit cast behavior") {
        current_t.type = SemanticType::u8;
        SECTION("cast to u8") {
            result_t.type = SemanticType::u8;
            REQUIRE(canCast(result_t, current_t) == true);
        }
        SECTION("cast to u16") {
            result_t.type = SemanticType::u16;
            REQUIRE(canCast(result_t, current_t) == true);
        }
        SECTION("cast to u32") {
            result_t.type = SemanticType::u32;
            REQUIRE(canCast(result_t, current_t) == true);
        }
        SECTION("cast to u64") {
            result_t.type = SemanticType::u64;
            REQUIRE(canCast(result_t, current_t) == true);
        }
        SECTION("cast to s8") {
            result_t.type = SemanticType::s8;
            REQUIRE(canCast(result_t, current_t) == false);
        }
        SECTION("cast to s16") {
            result_t.type = SemanticType::s16;
            REQUIRE(canCast(result_t, current_t) == true);
        }
        SECTION("cast to s32") {
            result_t.type = SemanticType::s32;
            REQUIRE(canCast(result_t, current_t) == true);
        }
        SECTION("cast to s64") {
            result_t.type = SemanticType::s64;
            REQUIRE(canCast(result_t, current_t) == true);
        }
    }
    SECTION("u16 implicit cast behavior") {
        current_t.type = SemanticType::u16;
        SECTION("cast to u8") {
            result_t.type = SemanticType::u8;
            REQUIRE(canCast(result_t, current_t) == false);
        }
        SECTION("cast to u16") {
            result_t.type = SemanticType::u16;
            REQUIRE(canCast(result_t, current_t) == true);
        }
        SECTION("cast to u32") {
            result_t.type = SemanticType::u32;
            REQUIRE(canCast(result_t, current_t) == true);
        }
        SECTION("cast to u64") {
            result_t.type = SemanticType::u64;
            REQUIRE(canCast(result_t, current_t) == true);
        }
        SECTION("cast to s8") {
            result_t.type = SemanticType::s8;
            REQUIRE(canCast(result_t, current_t) == false);
        }
        SECTION("cast to s16") {
            result_t.type = SemanticType::s16;
            REQUIRE(canCast(result_t, current_t) == false);
        }
        SECTION("cast to s32") {
            result_t.type = SemanticType::s32;
            REQUIRE(canCast(result_t, current_t) == true);
        }
        SECTION("cast to s64") {
            result_t.type = SemanticType::s64;
            REQUIRE(canCast(result_t, current_t) == true);
        }
    }
    SECTION("u32 implicit cast behavior") {
        current_t.type = SemanticType::u32;
        SECTION("cast to u8") {
            result_t.type = SemanticType::u8;
            REQUIRE(canCast(result_t, current_t) == false);
        }
        SECTION("cast to u16") {
            result_t.type = SemanticType::u16;
            REQUIRE(canCast(result_t, current_t) == false);
        }
        SECTION("cast to u32") {
            result_t.type = SemanticType::u32;
            REQUIRE(canCast(result_t, current_t) == true);
        }
        SECTION("cast to u64") {
            result_t.type = SemanticType::u64;
            REQUIRE(canCast(result_t, current_t) == true);
        }
        SECTION("cast to s8") {
            result_t.type = SemanticType::s8;
            REQUIRE(canCast(result_t, current_t) == false);
        }
        SECTION("cast to s16") {
            result_t.type = SemanticType::s16;
            REQUIRE(canCast(result_t, current_t) == false);
        }
        SECTION("cast to s32") {
            result_t.type = SemanticType::s32;
            REQUIRE(canCast(result_t, current_t) == false);
        }
        SECTION("cast to s64") {
            result_t.type = SemanticType::s64;
            REQUIRE(canCast(result_t, current_t) == true);
        }
    }
    SECTION("u64 to all Integer Types") {
        current_t.type = SemanticType::u64;
        SECTION("cast to u8") {
            result_t.type = SemanticType::u8;
            REQUIRE(canCast(result_t, current_t) == false);
        }
        SECTION("cast to u16") {
            result_t.type = SemanticType::u16;
            REQUIRE(canCast(result_t, current_t) == false);
        }
        SECTION("cast to u32") {
            result_t.type = SemanticType::u32;
            REQUIRE(canCast(result_t, current_t) == false);
        }
        SECTION("cast to u64") {
            result_t.type = SemanticType::u64;
            REQUIRE(canCast(result_t, current_t) == true);
        }
        SECTION("cast to s8") {
            result_t.type = SemanticType::s8;
            REQUIRE(canCast(result_t, current_t) == false);
        }
        SECTION("cast to s16") {
            result_t.type = SemanticType::s16;
            REQUIRE(canCast(result_t, current_t) == false);
        }
        SECTION("cast to s32") {
            result_t.type = SemanticType::s32;
            REQUIRE(canCast(result_t, current_t) == false);
        }
        SECTION("cast to s64") {
            result_t.type = SemanticType::s64;
            REQUIRE(canCast(result_t, current_t) == false);
        }
    }
    SECTION("s8 to all signed integer types") {
        current_t.type = SemanticType::s8;
        SECTION("cast to s8") {
            result_t.type = SemanticType::s8;
            REQUIRE(canCast(result_t, current_t) == true);
        }
        SECTION("cast to s16") {
            result_t.type = SemanticType::s16;
            REQUIRE(canCast(result_t, current_t) == true);
        }
        SECTION("cast to s32") {
            result_t.type = SemanticType::s32;
            REQUIRE(canCast(result_t, current_t) == true);
        }
        SECTION("cast to s64") {
            result_t.type = SemanticType::s64;
            REQUIRE(canCast(result_t, current_t) == true);
        }
    }
    SECTION("s16 to all signed integer types") {
        current_t.type = SemanticType::s16;
        SECTION("cast to s8") {
            result_t.type = SemanticType::s8;
            REQUIRE(canCast(result_t, current_t) == false);
        }
        SECTION("cast to s16") {
            result_t.type = SemanticType::s16;
            REQUIRE(canCast(result_t, current_t) == true);
        }
        SECTION("cast to s32") {
            result_t.type = SemanticType::s32;
            REQUIRE(canCast(result_t, current_t) == true);
        }
        SECTION("cast to s64") {
            result_t.type = SemanticType::s64;
            REQUIRE(canCast(result_t, current_t) == true);
        }
    }
    SECTION("s32 to all signed integer types") {
        current_t.type = SemanticType::s32;
        SECTION("cast to s8") {
            result_t.type = SemanticType::s8;
            REQUIRE(canCast(result_t, current_t) == false);
        }
        SECTION("cast to s16") {
            result_t.type = SemanticType::s16;
            REQUIRE(canCast(result_t, current_t) == false);
        }
        SECTION("cast to s32") {
            result_t.type = SemanticType::s32;
            REQUIRE(canCast(result_t, current_t) == true);
        }
        SECTION("cast to s64") {
            result_t.type = SemanticType::s64;
            REQUIRE(canCast(result_t, current_t) == true);
        }
    }
    SECTION("s64 to all signed integer types") {
        current_t.type = SemanticType::s64;
        SECTION("cast to s8") {
            result_t.type = SemanticType::s8;
            REQUIRE(canCast(result_t, current_t) == false);
        }
        SECTION("cast to s16") {
            result_t.type = SemanticType::s16;
            REQUIRE(canCast(result_t, current_t) == false);
        }
        SECTION("cast to s32") {
            result_t.type = SemanticType::s32;
            REQUIRE(canCast(result_t, current_t) == false);
        }
        SECTION("cast to s64") {
            result_t.type = SemanticType::s64;
            REQUIRE(canCast(result_t, current_t) == true);
        }
    }
    SECTION("All signed to All unsigned integers") {
        std::array<SemanticType,4> signedTypes {{
            SemanticType::s8,
            SemanticType::s16,
            SemanticType::s32,
            SemanticType::s64
        }};
        std::array<SemanticType,4> unsignedTypes {{
            SemanticType::u8,
            SemanticType::u16,
            SemanticType::u32,
            SemanticType::u64
        }};
        for(auto signed_t : signedTypes) {
            current_t.type = signed_t;
            for(auto usigned_t : unsignedTypes) {
                result_t.type = usigned_t;
                REQUIRE(canCast(result_t, current_t) == false);
            }
        }
    }
}
