#ifndef PARTICLE_SYSTEM_VULKAN_UITEXTS_HPP
#define PARTICLE_SYSTEM_VULKAN_UITEXTS_HPP

namespace UiTexts {

constexpr char const *INPUT_PARTICLES_NB_WIN_NAME = "Set number of particles";
constexpr char const *INPUT_PARTICLES_NB_WIN_TEXT = "Particles";

constexpr char const *INPUT_PARTICLES_SPEED_WIN_NAME =
  "Set particles max speed";
constexpr char const *INPUT_PARTICLES_SPEED_WIN_TEXT = "m/s";

constexpr char const *INPUT_PARTICLES_COLOR_WIN_NAME =
  "Particles color selection";

constexpr char const *INPUT_PARTICLES_HELP_WIN_NAME = "Help";
constexpr char const *INPUT_PARTICLES_HELP_WIN_TEXT =
  "Controls:\n"
  "\tLeft click: set gravity center\n"
  "\tScroll up: increase gravity\n"
  "\tScroll down: decrease gravity\n"
  "\tWASD: move when camera mvt is active (F4)";

constexpr char const *INPUT_PARTICLES_ERROR_WIN_NAME = "Error";

}

#endif // PARTICLE_SYSTEM_VULKAN_UITEXTS_HPP
