#version 330

// Input texture coordinates and texture sampler
in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture0;  // Texture being modified
uniform vec4 colorOffset;   // RGBA color offsets

out vec4 finalColor;

void main()
{
    vec4 texColor = texture(texture0, fragTexCoord);  // Sample the texture color
    vec4 modifiedColor = texColor + colorOffset;      // Apply color offset
    finalColor = clamp(modifiedColor, 0.0, 1.0);      // Clamp values to valid range
}
