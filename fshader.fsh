uniform sampler2D u_texture;
uniform highp vec4 u_lightPosition;
uniform highp float u_lightPower;
varying highp vec4 v_position;
varying highp vec3 v_normal;
varying highp vec2 v_textcoord;

void main(void)
{
    float specularFactor = 60.; // Размер пятна блика. Зеркальная составляющая.
    float ambientFactor = 0.1;   // Коэффициент свечения материала. Фоновая составляющая.
    vec4 result = vec4(0., 0., 0., 0.);
    vec4 eyePosition = vec4(0., 0., 0., 1.);
    vec4 diffMatColor = texture2D(u_texture, v_textcoord); // Диффузный цвет материала.
    vec3 eyeVect = normalize(v_position.xyz - eyePosition.xyz); // Вектор наблюдения.
    vec3 lightVect = normalize(v_position.xyz - u_lightPosition.xyz); // Вектор освещения.
    vec3 reflectLight = normalize(reflect(lightVect, v_normal)); // Отраженный свет.
    float len = length(v_position.xyz - eyePosition.xyz); // Длина вектора от наблюдателя к рассматриваемой точке

    vec4 diffColor = diffMatColor * u_lightPower * max(0., dot(v_normal, -lightVect)); // / (1. + 0.25 * pow(len, 2.)); Необходимо для того, чтобы с удалением, детали освещались хуже.
    result += diffColor;
    vec4 ambientColor = ambientFactor * diffMatColor;
    result += ambientColor;
    vec4 specularColor = vec4(0., 0., 0., 1.) * u_lightPower * pow(max(0., dot(reflectLight, -eyeVect)),specularFactor); // / (1. + 0.25 * pow(len, 2.));
    result += specularColor;
    gl_FragColor = result;//texture2D(u_texture, v_textcoord); //vec4(1., 0., 0., 1.);
    // встроенная переменная, в которую записывается обработанный фрагментным шейдером цвет фрагмента и которая и заменяет фрагмент графического конвеера.
}
