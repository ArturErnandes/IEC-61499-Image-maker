// переводит вектор в пнг
#include "png_renderer.hpp"
#include <cairo.h>
#include <librsvg/rsvg.h>
#include <stdexcept>
#include <memory>

namespace fbsvg {

void PNGRenderer::render_from_svg(const std::string &svgContent, const std::string &filename) {

    rsvg_init();

    GError *error = nullptr;
    std::unique_ptr<RsvgHandle, decltype(&g_object_unref)>
        handle(rsvg_handle_new_from_data((const guint8*)svgContent.c_str(), svgContent.size(), &error),
               g_object_unref);
    if (!handle) throw std::runtime_error("Failed to parse SVG: " + std::string(error->message));

    RsvgDimensionData dim;
    rsvg_handle_get_dimensions(handle.get(), &dim);

    cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, dim.width, dim.height);
    cairo_t *cr = cairo_create(surface);

    if (!rsvg_handle_render_cairo(handle.get(), cr)) {
        cairo_destroy(cr);
        cairo_surface_destroy(surface);
        throw std::runtime_error("Failed to render SVG to Cairo surface");
    }


    cairo_surface_write_to_png(surface, filename.c_str());

    cairo_destroy(cr);
    cairo_surface_destroy(surface);

    rsvg_term();
}

}
