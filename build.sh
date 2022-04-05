#!/bin/bash
#set -e

pushd build &>/dev/null

while getopts p: flag; do
    case "${flag}" in
    p)
        shift 2
        platform=${OPTARG}
        ;;
    \?) help ;;
    esac
done

help() {
    echo build.sh - build debug library
    echo build.sh clean - clean the build
    echo build.sh release - build release library
    echo build.sh -p ios release - build release ios library
    echo build.sh -p android release - build release android library
    exit 1
}

# make sure argument is lowercase
OPTION="$(echo "$1" | tr '[A-Z]' '[a-z]')"

if [ "$OPTION" = 'help' ]; then
    help
else
    build() {
        echo "Building Rive for platform=$platform option=$OPTION"
        echo premake5 gmake2 "$1"
        PREMAKE="premake5 gmake2 $1"
        eval "$PREMAKE"
        if [ "$OPTION" = "clean" ]; then
            make clean
            make clean config=release
        elif [ "$OPTION" = "release" ]; then
            verbose=1 make config=release -j7
            echo ATTEMPT IT WINDOWS
            ar -rcsv "windows/bin/release/rive.lib"  windows/obj/release/aabb.o windows/obj/release/animation_base.o windows/obj/release/animation_state.o windows/obj/release/animation_state_base.o windows/obj/release/animation_state_instance.o windows/obj/release/any_state_base.o windows/obj/release/artboard.o windows/obj/release/artboard_base.o windows/obj/release/artboard_importer.o windows/obj/release/backboard_base.o windows/obj/release/backboard_importer.o windows/obj/release/binary_reader.o windows/obj/release/blend_animation.o windows/obj/release/blend_animation_1d.o windows/obj/release/blend_animation_1d_base.o windows/obj/release/blend_animation_direct.o windows/obj/release/blend_animation_direct_base.o windows/obj/release/blend_state.o windows/obj/release/blend_state_1d.o windows/obj/release/blend_state_1d_base.o windows/obj/release/blend_state_1d_instance.o windows/obj/release/blend_state_direct.o windows/obj/release/blend_state_direct_base.o windows/obj/release/blend_state_direct_instance.o windows/obj/release/blend_state_transition.o windows/obj/release/blend_state_transition_base.o windows/obj/release/bone.o windows/obj/release/bone_base.o windows/obj/release/clipping_shape.o windows/obj/release/clipping_shape_base.o windows/obj/release/color.o windows/obj/release/component.o windows/obj/release/constraint.o windows/obj/release/contour_mesh_vertex_base.o windows/obj/release/core_bool_type.o windows/obj/release/core_bytes_type.o windows/obj/release/core_color_type.o windows/obj/release/core_double_type.o windows/obj/release/core_string_type.o windows/obj/release/core_uint_type.o windows/obj/release/cubic_asymmetric_vertex.o windows/obj/release/cubic_asymmetric_vertex_base.o windows/obj/release/cubic_detached_vertex.o windows/obj/release/cubic_detached_vertex_base.o windows/obj/release/cubic_interpolator.o windows/obj/release/cubic_interpolator_base.o windows/obj/release/cubic_mirrored_vertex.o windows/obj/release/cubic_mirrored_vertex_base.o windows/obj/release/cubic_vertex.o windows/obj/release/cubic_weight_base.o windows/obj/release/dependency_sorter.o windows/obj/release/distance_constraint.o windows/obj/release/distance_constraint_base.o windows/obj/release/draw_rules.o windows/obj/release/draw_rules_base.o windows/obj/release/draw_target.o windows/obj/release/draw_target_base.o windows/obj/release/drawable.o windows/obj/release/ellipse.o windows/obj/release/ellipse_base.o windows/obj/release/entry_state_base.o windows/obj/release/event_bool_change.o windows/obj/release/event_bool_change_base.o windows/obj/release/event_input_change.o windows/obj/release/event_number_change.o windows/obj/release/event_number_change_base.o windows/obj/release/event_trigger_change.o windows/obj/release/event_trigger_change_base.o windows/obj/release/exit_state_base.o windows/obj/release/file.o windows/obj/release/file_asset.o windows/obj/release/file_asset_contents.o windows/obj/release/file_asset_contents_base.o windows/obj/release/file_asset_importer.o windows/obj/release/fill.o windows/obj/release/fill_base.o windows/obj/release/folder_base.o windows/obj/release/gradient_stop.o windows/obj/release/gradient_stop_base.o windows/obj/release/hit_test.o windows/obj/release/hittest_command_path.o windows/obj/release/ik_constraint.o windows/obj/release/ik_constraint_base.o windows/obj/release/image.o windows/obj/release/image_asset.o windows/obj/release/image_asset_base.o windows/obj/release/image_base.o windows/obj/release/keyed_object.o windows/obj/release/keyed_object_base.o windows/obj/release/keyed_object_importer.o windows/obj/release/keyed_property.o windows/obj/release/keyed_property_base.o windows/obj/release/keyed_property_importer.o windows/obj/release/keyframe.o windows/obj/release/keyframe_bool.o windows/obj/release/keyframe_bool_base.o windows/obj/release/keyframe_color.o windows/obj/release/keyframe_color_base.o windows/obj/release/keyframe_double.o windows/obj/release/keyframe_double_base.o windows/obj/release/keyframe_id.o windows/obj/release/keyframe_id_base.o windows/obj/release/layer_state.o windows/obj/release/layer_state_importer.o windows/obj/release/layout.o windows/obj/release/linear_animation.o windows/obj/release/linear_animation_base.o windows/obj/release/linear_animation_importer.o windows/obj/release/linear_animation_instance.o windows/obj/release/linear_gradient.o windows/obj/release/linear_gradient_base.o windows/obj/release/mat2d.o windows/obj/release/mesh.o windows/obj/release/mesh_base.o windows/obj/release/mesh_vertex.o windows/obj/release/mesh_vertex_base.o windows/obj/release/metrics_path.o windows/obj/release/nested_animation.o windows/obj/release/nested_artboard.o windows/obj/release/nested_artboard_base.o windows/obj/release/nested_linear_animation.o windows/obj/release/nested_remap_animation.o windows/obj/release/nested_remap_animation_base.o windows/obj/release/nested_simple_animation.o windows/obj/release/nested_simple_animation_base.o windows/obj/release/nested_state_machine.o windows/obj/release/nested_state_machine_base.o windows/obj/release/node.o windows/obj/release/node_base.o windows/obj/release/parametric_path.o windows/obj/release/path.o windows/obj/release/path_composer.o windows/obj/release/path_vertex.o windows/obj/release/points_path.o windows/obj/release/points_path_base.o windows/obj/release/polygon.o windows/obj/release/polygon_base.o windows/obj/release/radial_gradient.o windows/obj/release/radial_gradient_base.o windows/obj/release/raw_path.o windows/obj/release/rectangle.o windows/obj/release/rectangle_base.o windows/obj/release/renderer.o windows/obj/release/root_bone.o windows/obj/release/root_bone_base.o windows/obj/release/rotation_constraint.o windows/obj/release/rotation_constraint_base.o windows/obj/release/scale_constraint.o windows/obj/release/scale_constraint_base.o windows/obj/release/shape.o windows/obj/release/shape_base.o windows/obj/release/shape_paint.o windows/obj/release/shape_paint_container.o windows/obj/release/shape_paint_mutator.o windows/obj/release/skin.o windows/obj/release/skin_base.o windows/obj/release/skinnable.o windows/obj/release/solid_color.o windows/obj/release/solid_color_base.o windows/obj/release/star.o windows/obj/release/star_base.o windows/obj/release/state_instance.o windows/obj/release/state_machine.o windows/obj/release/state_machine_base.o windows/obj/release/state_machine_bool_base.o windows/obj/release/state_machine_event.o windows/obj/release/state_machine_event_base.o windows/obj/release/state_machine_event_importer.o windows/obj/release/state_machine_importer.o windows/obj/release/state_machine_input.o windows/obj/release/state_machine_input_instance.o windows/obj/release/state_machine_instance.o windows/obj/release/state_machine_layer.o windows/obj/release/state_machine_layer_base.o windows/obj/release/state_machine_layer_importer.o windows/obj/release/state_machine_number_base.o windows/obj/release/state_machine_trigger_base.o windows/obj/release/state_transition.o windows/obj/release/state_transition_base.o windows/obj/release/state_transition_importer.o windows/obj/release/straight_vertex.o windows/obj/release/straight_vertex_base.o windows/obj/release/stroke.o windows/obj/release/stroke_base.o windows/obj/release/system_state_instance.o windows/obj/release/targeted_constraint.o windows/obj/release/tendon.o windows/obj/release/tendon_base.o windows/obj/release/transform_component.o windows/obj/release/transform_constraint.o windows/obj/release/transform_constraint_base.o windows/obj/release/transition_bool_condition.o windows/obj/release/transition_bool_condition_base.o windows/obj/release/transition_condition.o windows/obj/release/transition_number_condition.o windows/obj/release/transition_number_condition_base.o windows/obj/release/transition_trigger_condition.o windows/obj/release/transition_trigger_condition_base.o windows/obj/release/translation_constraint.o windows/obj/release/translation_constraint_base.o windows/obj/release/triangle.o windows/obj/release/triangle_base.o windows/obj/release/trim_path.o windows/obj/release/trim_path_base.o windows/obj/release/vec2d.o windows/obj/release/vertex.o windows/obj/release/weight.o windows/obj/release/weight_base.o windows/obj/release/world_transform_component.o
        else
            verbose=1 make -j7
        fi
    }

    case $platform in
    ios)
        echo "Building for iOS"
        export IOS_SYSROOT=$(xcrun --sdk iphoneos --show-sdk-path)
        build "--os=ios"
        export IOS_SYSROOT=$(xcrun --sdk iphonesimulator --show-sdk-path)
        build "--os=ios --variant=emulator"
        if [ "$OPTION" = "clean" ]; then
            exit
        fi
        ;;
    # Android supports ABIs via a custom platform format:
    #   e.g. 'android.x86', 'android.x64', etc.
    android*)
        echo "Building for ${platform}"
        # Extract ABI from this opt by splitting on '.' character
        #   e.g. android.x86
        IFS="." read -ra strarr <<<"$platform"
        ARCH=${strarr[1]}
        build "--os=android --arch=${ARCH}"
        ;;
    *)
        build
        ;;
    esac
fi

popd &>/dev/null
