// переводит вектор в пнг
#include "png_renderer.hpp"
#include <cairo.h>
#include <librsvg/rsvg.h>

namespace fbsvg {

void PNGRenderer::render_from_svg(const std::string &svgContent, const std::string &filename) {

    rsvg_init();

    RsvgHandle *handle = rsvg_handle_new_from_data(
        reinterpret_cast<const guint8*>(svgContent.data()),
        svgContent.size(),
        nullptr 
    );

    RsvgDimensionData dim;
    rsvg_handle_get_dimensions(handle, &dim);

    cairo_surface_t *surface = cairo_image_surface_create(
        CAIRO_FORMAT_ARGB32,
        dim.width,
        dim.height
    );

    cairo_t *cr = cairo_create(surface);

    rsvg_handle_render_cairo(handle, cr);

    cairo_surface_write_to_png(surface, filename.c_str());

    cairo_destroy(cr);
    cairo_surface_destroy(surface);

    g_object_unref(handle);
    rsvg_term();
}

}
