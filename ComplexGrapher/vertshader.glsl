#version 330 core

layout (location = 0) in vec2 aPos;
uniform int uPointSize = 4;
uniform float uZoom = 1;
uniform vec2 uPos = vec2(0, 0);

void main() {
	gl_PointSize = uPointSize;
	gl_Position = vec4(aPos.x * uZoom, aPos.y * uZoom, 1, 1.0);
}