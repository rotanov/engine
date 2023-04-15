$input v_color0, v_texcoord0

/*
 * Copyright 2011-2023 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bgfx/blob/master/LICENSE
 */

#include "common.sh"

SAMPLER2D(s_texColor, 0);

void main()
{
  vec4 rgba = texture2D(s_texColor, v_texcoord0);
  gl_FragColor = toLinear(rgba) * toLinear(v_color0);
}