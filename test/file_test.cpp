#include <rive/file.hpp>
#include <rive/node.hpp>
#include <rive/shapes/rectangle.hpp>
#include <rive/shapes/shape.hpp>
#include "no_op_renderer.hpp"
#include "rive_file_reader.hpp"
#include <catch.hpp>
#include <cstdio>

TEST_CASE("file can be read", "[file]") {
    RenderObjectLeakChecker checker;
    auto file = ReadRiveFile("../../test/assets/two_artboards.riv");

    // Default artboard should be named Two.
    REQUIRE(file->artboard()->name() == "Two");

    // There should be a second artboard named One.
    REQUIRE(file->artboard("One") != nullptr);
}

TEST_CASE("file with animation can be read", "[file]") {
    RenderObjectLeakChecker checker;
    auto file = ReadRiveFile("../../test/assets/juice.riv");

    auto artboard = file->artboard();
    REQUIRE(artboard->name() == "New Artboard");

    auto shin = artboard->find("shin_right");
    REQUIRE(shin != nullptr);
    REQUIRE(shin->is<rive::Node>());

    auto shinNode = shin->as<rive::Node>();
    REQUIRE(shinNode->parent() != nullptr);
    REQUIRE(shinNode->parent()->name() == "leg_right");
    REQUIRE(shinNode->parent()->parent() != nullptr);
    REQUIRE(shinNode->parent()->parent()->name() == "root");
    REQUIRE(shinNode->parent()->parent() != nullptr);
    REQUIRE(shinNode->parent()->parent()->parent() != nullptr);
    REQUIRE(shinNode->parent()->parent()->parent() == artboard);

    auto walkAnimation = artboard->animation("walk");
    REQUIRE(walkAnimation != nullptr);
    REQUIRE(walkAnimation->numKeyedObjects() == 22);
}

TEST_CASE("artboards can be counted and accessed via index or name", "[file]") {
    RenderObjectLeakChecker checker;
    auto file = ReadRiveFile("../../test/assets/dependency_test.riv");

    // The artboards caqn be counted
    REQUIRE(file->artboardCount() == 1);

    // Artboards can be access by index
    REQUIRE(file->artboard(0) != nullptr);

    // Artboards can be accessed by name
    REQUIRE(file->artboard("Blue") != nullptr);
}

TEST_CASE("dependencies are as expected", "[file]") {
    RenderObjectLeakChecker checker;
    // ┌────┐
    // │Blue│
    // └────┘
    //    │ ┌───┐
    //    └▶│ A │
    //      └───┘
    //        │ ┌───┐
    //        └▶│ B │
    //          └───┘
    //            │ ┌───┐
    //            ├▶│ C │
    //            │ └───┘
    //            │ ┌─────────┐
    //            └▶│Rectangle│
    //              └─────────┘
    //                   │ ┌──────────────┐
    //                   └▶│Rectangle Path│
    //                     └──────────────┘
    auto file = ReadRiveFile("../../test/assets/dependency_test.riv");

    auto artboard = file->artboard();
    REQUIRE(artboard->name() == "Blue");

    auto nodeA = artboard->find<rive::Node>("A");
    auto nodeB = artboard->find<rive::Node>("B");
    auto nodeC = artboard->find<rive::Node>("C");
    auto shape = artboard->find<rive::Shape>("Rectangle");
    auto path = artboard->find<rive::Path>("Rectangle Path");
    REQUIRE(nodeA != nullptr);
    REQUIRE(nodeB != nullptr);
    REQUIRE(nodeC != nullptr);
    REQUIRE(shape != nullptr);
    REQUIRE(path != nullptr);

    REQUIRE(nodeA->parent() == artboard);
    REQUIRE(nodeB->parent() == nodeA);
    REQUIRE(nodeC->parent() == nodeB);
    REQUIRE(shape->parent() == nodeB);
    REQUIRE(path->parent() == shape);

    REQUIRE(nodeB->dependents().size() == 2);

    REQUIRE(artboard->graphOrder() == 0);
    REQUIRE(nodeA->graphOrder() > artboard->graphOrder());
    REQUIRE(nodeB->graphOrder() > nodeA->graphOrder());
    REQUIRE(nodeC->graphOrder() > nodeB->graphOrder());
    REQUIRE(shape->graphOrder() > nodeB->graphOrder());
    REQUIRE(path->graphOrder() > shape->graphOrder());

    artboard->advance(0.0f);

    auto world = shape->worldTransform();
    REQUIRE(world[4] == 39.203125f);
    REQUIRE(world[5] == 29.535156f);
}

TEST_CASE("long name in object is parsed correctly", "[file]") {
    RenderObjectLeakChecker checker;
    auto file = ReadRiveFile("../../test/assets/long_name.riv");
    auto artboard = file->artboard();

    // Expect all object in file to be loaded, in this case 7
    REQUIRE(artboard->objects().size() == 7);
}

// TODO:
// ShapePaint (fill/stroke) needs to be implemented in WASM (jsFill/jsStroke) in
// order to create Paint objects as necessary.

// Mutators need to be implemented in WASM (solid/linear/radial) and get access
// to their ShapePaint so they can mutate any extra objects they create on it
// (like a paint object for skia).

// Paths need to be implemented in WASM but not so much as a core path (like
// parametric/pointspath, etc) but more as a general rendering path. Handed
// their commands so they can generate/store a re-usable path. This would be a
// Path2D in context2D and a SkPath in CanvasKit.

// PathComposer is the factory for the Paths. But they do need to surive so they
// can be reset/reused as available by the rendering lib.

// PathComposer needs to be implemented in WASM to compose the paths together
// and be accessible from the Shape (jsShape) which will need a call
// setupFill/restoreFill and setupStroke/restoreStroke.

// Draw will be called by C++ on the Shape, the Shape will call draw on the
// fill/stroke (propagates to jsFill/jsStroke)
