#include "debug.h"

#include "../../thirdparty/c_log.h"

const char *debug_get_err_source_name(GLenum source) {
    switch (source) {
        case GL_DEBUG_SOURCE_API:               return "API";
        case GL_DEBUG_SOURCE_APPLICATION:       return "Application";
        case GL_DEBUG_SOURCE_SHADER_COMPILER:   return "Shader Compiler";
        case GL_DEBUG_SOURCE_THIRD_PARTY:       return "Third Party";
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:     return "Window System";
        case GL_DEBUG_SOURCE_OTHER:             return "Other";
        default:                                return "Unknown Source";
    }
}

const char *debug_get_err_type_name(GLenum type) {
    switch (type) {
        case GL_DEBUG_TYPE_ERROR:               return "Error";
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "Deprecated Behaviour";
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  return "Undefined Behaviour";
        case GL_DEBUG_TYPE_PORTABILITY:         return "Portability";
        case GL_DEBUG_TYPE_PERFORMANCE:         return "Performance";
        case GL_DEBUG_TYPE_MARKER:              return "Marker";
        case GL_DEBUG_TYPE_PUSH_GROUP:          return "Push Group";
        case GL_DEBUG_TYPE_POP_GROUP:           return "Pop Group";
        case GL_DEBUG_TYPE_OTHER:               return "Other";
        default:                                return "Unknown Type";
    }
}

void debug_message(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                  const GLchar *message, const void *user_param)
{
    CLogSeverity log_severity;
    if (severity == GL_DEBUG_SEVERITY_HIGH)         log_severity = C_LOG_SEVERITY_ERROR;
    if (severity == GL_DEBUG_SEVERITY_MEDIUM)       log_severity = C_LOG_SEVERITY_WARNING;
    if (severity == GL_DEBUG_SEVERITY_LOW)          log_severity = C_LOG_SEVERITY_INFO;
    if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) log_severity = C_LOG_SEVERITY_INFO;

    c_log(log_severity, "OpenGL Error (ID = %d, Source = %s, Type = %s): %s",
          id, debug_get_err_source_name(source), debug_get_err_type_name(type), message);
}

void debug_register_message_callback() {
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(debug_message, NULL);

    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
}
