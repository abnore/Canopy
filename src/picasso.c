#include "picasso.h"
#include "logger.h"

#include <errno.h>

#define return_defer(value) do { result = (value); goto defer; } while(0)

#define X(x) color_to_u32(x)
const char* color_to_string(color c)
{
    uint32_t value = X(c);
    switch (value) {
        case X(BLUE):        return "BLUE";
        case X(GREEN):       return "GREEN";
        case X(RED):         return "RED";

        case X(WHITE):       return "WHITE";
        case X(BLACK):       return "BLACK";
        case X(GRAY):        return "GRAY";
        case X(LIGHT_GRAY):  return "LIGHT_GRAY";
        case X(DARK_GRAY):   return "DARK_GRAY";

        case X(ORANGE):      return "ORANGE";
        case X(YELLOW):      return "YELLOW";
        case X(BROWN):       return "BROWN";
        case X(GOLD):        return "GOLD";

        case X(CYAN):        return "CYAN";
        case X(MAGENTA):     return "MAGENTA";
        case X(PURPLE):      return "PURPLE";
        case X(NAVY):        return "NAVY";
        case X(TEAL):        return "TEAL";

        default: return "UNKNOWN";
    }
}
#undef X

// WIP - not robust enough
BMP *picasso_load_bmp(const char *filename)
{
    BMP *image = canopy_malloc(sizeof(BMP));
    if (!image) {
        fprintf(stderr, "Failed to allocate BMP struct\n");
        return NULL;
    }
    memset(image, 0, sizeof(BMP));

    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Unable to open file");
        canopy_free(image);
        return NULL;
    }

    // Read File Header
    fread(&image->fh, sizeof(image->fh), 1, file);
    if (image->fh.file_type != 0x4D42) {
        fprintf(stderr, "Not a valid BMP file (magic: 0x%X)\n", image->fh.file_type);
        fclose(file);
        canopy_free(image);
        return NULL;
    }

    // Read Info Header
    fread(&image->ih, sizeof(image->ih), 1, file);

    // Move to pixel data
    fseek(file, image->fh.offset_data, SEEK_SET);

    // Allocate pixel data buffer
    image->pixels = canopy_malloc(image->ih.size_image);
    if (!image->pixels) {
        fprintf(stderr, "Failed to allocate BMP pixel buffer\n");
        fclose(file);
        canopy_free(image);
        return NULL;
    }

    // Read pixel data
    fread(image->pixels, image->ih.size_image, 1, file);
    fclose(file);

    // Convert from BGRA to RGBA (macOS expects RGBA)
    for (int i = 0; i < image->ih.width * image->ih.height; ++i) {
        uint8_t *p = &image->pixels[i * 4];
        uint8_t tmp = p[0];
        p[0] = p[2];  // Swap B and R
        p[2] = tmp;
    }

    return image;
}

void picasso_flip_buffer_vertical(uint8_t *buffer, int width, int height) {
    int bytes_per_pixel = 4; // RGBA
    int row_size = width * bytes_per_pixel;

    uint8_t temp_row[row_size]; // Temporary row buffer

    for (int y = 0; y < height / 2; y++) {
        int top_index = y * row_size;
        int bottom_index = (height - y - 1) * row_size;

        // Swap the rows
        memcpy(temp_row, &buffer[top_index], row_size);
        memcpy(&buffer[top_index], &buffer[bottom_index], row_size);
        memcpy(&buffer[bottom_index], temp_row, row_size);
    }
}

PPM *picasso_load_ppm(const char *file_path)
{
    return NULL;
}

int picasso_save_to_ppm(PPM *image, const char *file_path)
{
	int result = 0;
	FILE *f = NULL;

	{
		f = fopen(file_path, "wb");
		if (f == NULL) return_defer(errno);

		fprintf(f, "P6\n%zu %zu 255\n", image->width, image->height);
		if (ferror(f)) return_defer(errno);

		for (size_t i = 0; i < image->width*image->height; i++) {
			// 0xAABBGGRR - skipping alpha
			uint32_t pixel = image->pixels[i];
			uint8_t bytes[3] = {
				(pixel>>(8*0)) & 0xFF, // Red
				(pixel>>(8*1)) & 0xFF, // Green
				(pixel>>(8*2)) & 0xFF  // Blue
			};
			fwrite(bytes, sizeof(bytes), 1, f);
			if (ferror(f)) return_defer(errno);
		}
	}

defer:
	if (f) fclose(f);
	return result;
}


// SPRITES

picasso_sprite_sheet* picasso_create_sprite_sheet(
    uint32_t* pixels,
    int sheet_width,
    int sheet_height,
    int frame_width,
    int frame_height,
    int margin_x,
    int margin_y,
    int spacing_x,
    int spacing_y)
{
    if (!pixels || frame_width <= 0 || frame_height <= 0) return NULL;

    int cols = (sheet_width  - 2 * margin_x + spacing_x) / (frame_width + spacing_x);
    int rows = (sheet_height - 2 * margin_y + spacing_y) / (frame_height + spacing_y);
    int total = cols * rows;

    picasso_sprite_sheet* sheet = canopy_malloc(sizeof(picasso_sprite_sheet));
    if (!sheet) return NULL;

    sheet->pixels         = pixels;
    sheet->sheet_width    = sheet_width;
    sheet->sheet_height   = sheet_height;
    sheet->frame_width    = frame_width;
    sheet->frame_height   = frame_height;
    sheet->margin_x       = margin_x;
    sheet->margin_y       = margin_y;
    sheet->spacing_x      = spacing_x;
    sheet->spacing_y      = spacing_y;
    sheet->frames_per_row = cols;
    sheet->frames_per_col = rows;
    sheet->frame_count    = total;

    sheet->frames = canopy_malloc(sizeof(picasso_sprite) * total);
    if (!sheet->frames) {
        canopy_free(sheet);
        return NULL;
    }

    int i = 0;
    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            int x = margin_x + col * (frame_width + spacing_x);
            int y = margin_y + row * (frame_height + spacing_y);
            sheet->frames[i++] = (picasso_sprite){ x, y, frame_width, frame_height };
        }
    }

    return sheet;
}

void picasso_destroy_sprite_sheet(picasso_sprite_sheet* sheet)
{
    if (!sheet) return;
    canopy_free(sheet->frames);
    canopy_free(sheet);
}

// --------------------------------------------------------
// Graphical functions and utilities
// --------------------------------------------------------
/* Here we are supporting negative width and height, drawing
 * in all directions! */
static void picasso__normalize_rect(picasso_rect *r)
{
    if(!r) {WARN("Tried to normalize a NULL object");return;}
    if (r->width < 0) {
        r->x += r->width;
        r->width = -r->width;
    }
    if (r->height < 0) {
        r->y += r->height;
        r->height = -r->height;
    }
}
/* Creating the bounds for looping over, removing the logic from the draw functions */
static bool picasso__clip_rect_to_bounds(picasso_backbuffer *bf, const picasso_rect *r,
                                 picasso_draw_bounds *db)
{
    if (!r || r->width == 0 || r->height == 0)
        return false;

    if (r->x >= (int)bf->width || r->y >= (int)bf->height ||
        r->x + r->width <= 0 || r->y + r->height <= 0)
        return false;

    db->x0 = (r->x > 0) ? r->x : 0;
    db->y0 = (r->y > 0) ? r->y : 0;
    db->x1 = (r->x + r->width < bf->width) ? r->x + r->width : bf->width;
    db->y1 = (r->y + r->height < bf->height) ? r->y + r->height : bf->height;

    return true;
}

static inline uint32_t picasso__blend_pixel(uint32_t dst, uint32_t src)
{
    color back = u32_to_color(dst);
    color front = u32_to_color(src);

    uint8_t sa = GET_ALPHA(front); //(src >> 24) & 0xFF;
    if (sa == 0xff) return src;
    if (sa == 0x00) return dst;

    uint8_t sr = GET_RED(front); //src & 0xFF;
    uint8_t sg = GET_GREEN(front); //(src >> 8) & 0xFF;
    uint8_t sb = GET_BLUE(front); //(src >> 16) & 0xFF;

    uint8_t dr = GET_RED(back); //dst & 0xFF;
    uint8_t dg = GET_GREEN(back); //(dst >> 8) & 0xFF;
    uint8_t db = GET_BLUE(back); //(dst >> 16) & 0xFF;

    // alpha blending formula
    uint8_t r = (sr * sa + dr * (255 - sa)) / 255;
    uint8_t g = (sg * sa + dg * (255 - sa)) / 255;
    uint8_t b = (sb * sa + db * (255 - sa)) / 255;

    color ret = (color){r, g, b, 0xff};

    return color_to_u32(ret);
    //return (0xFF << 24) | (b << 16) | (g << 8) | r;
}


// --------------------------------------------------------
// Backbuffer operations
// --------------------------------------------------------


picasso_backbuffer* picasso_create_backbuffer(int width, int height)
{
    if (width <= 0 || height <= 0) {
        return NULL;
    }

    picasso_backbuffer* bf = malloc(sizeof(picasso_backbuffer));
    if (!bf) return NULL;

    bf->width = width;
    bf->height = height;
    bf->pitch = width * sizeof(uint32_t); // 4 bytes per pixel
    bf->pixels = calloc(width * height, sizeof(uint32_t));

    if (!bf->pixels) {
        free(bf);
        return NULL;
    }

    return bf;
}

void picasso_destroy_backbuffer(picasso_backbuffer* bf)
{
    if (!bf) return;
    if (bf->pixels) {
        free(bf->pixels);
        bf->pixels = NULL;
    }
    free(bf);
}

void* picasso_backbuffer_pixels(picasso_backbuffer* bf)
{
    if (!bf) return NULL;
    return (void*)bf->pixels;
}

void picasso_clear_backbuffer(picasso_backbuffer* bf)
{
    if (!bf || !bf->pixels) {
        WARN("Attempted to clear NULL backbuffer");
        return;
    }

    for (size_t i = 0; i <  bf->width * bf->height; ++i) {
        bf->pixels[i] = color_to_u32(CLEAR_BACKGROUND);
    }
}

void picasso_blit_bitmap(picasso_backbuffer* dst,
                         void* src_pixels, int src_w, int src_h,
                         int x, int y)
{
    if (!dst || !src_pixels || !dst->pixels) return;

    int dst_w = dst->width;
    int dst_h = dst->height;

    for (int row = 0; row < src_h; ++row)
    {
        int dst_y = y + row;
        if (dst_y < 0 || dst_y >= dst_h) continue;

        for (int col = 0; col < src_w; ++col)
        {
            int dst_x = x + col;
            if (dst_x < 0 || dst_x >= dst_w) continue;

            uint32_t* src = (uint32_t*)src_pixels;
            uint32_t* dst_pixel = &dst->pixels[dst_y * dst->width + dst_x];
            uint32_t  src_pixel = src[row * src_w + col];

            *dst_pixel = picasso__blend_pixel(*dst_pixel, src_pixel);
        }
    }
}




// --------------------------------------------------------
// Graphical primitives
// --------------------------------------------------------

void picasso_fill_rect(picasso_backbuffer *bf, picasso_rect *r, color c)
{
    picasso_draw_bounds bounds = {0};
    picasso__normalize_rect(r);
    if(!picasso__clip_rect_to_bounds(bf, r, &bounds)) return;

    uint32_t new_pixel = color_to_u32(c);

    for (int y = bounds.y0; y < bounds.y1; ++y) {
        for (int x = bounds.x0; x < bounds.x1; ++x) {
            uint32_t *cur_pixel = &bf->pixels[y * bf->width + x];
            *cur_pixel = picasso__blend_pixel(*cur_pixel, new_pixel);
        }
    }
}

/*
 * FIX: This approach might be slightly wasteful, but it works!
 * I calculate the whole rectangle and set outer bounds, and then i calculate an inner
 * rectangle which is the thickness less in - i then set that to be skipped.
 * So in practive i am going over every pixel of the larger rect, and skipping the inside
 *
 * I might be able to do a 4-slice instead
 * */
void picasso_draw_rect(picasso_backbuffer *bf, picasso_rect *outer, int thickness, color c)
{
    if (!outer || !bf || thickness <= 0) return;

    picasso_draw_bounds outer_bounds = {0};
    picasso_draw_bounds inner_bounds = {0};

    // Normalize original rectangle (respecting negative width/height)
    picasso__normalize_rect(outer);

    // Compute inner rectangle
    picasso_rect inner = {
        .x = outer->x + thickness,
        .y = outer->y + thickness,
        .width = outer->width - 2 * thickness,
        .height = outer->height - 2 * thickness
    };

    // Clip to draw bounds
    if (!picasso__clip_rect_to_bounds(bf, outer, &outer_bounds)) return;

    if (!picasso__clip_rect_to_bounds(bf, &inner, &inner_bounds)) {
        inner_bounds = (picasso_draw_bounds){0};
    }

    uint32_t new_pixel = color_to_u32(c);

    for (int y = outer_bounds.y0; y < outer_bounds.y1; ++y) {
        for (int x = outer_bounds.x0; x < outer_bounds.x1; ++x) {

            bool inside_inner = (
                    y >= inner_bounds.y0 && y < inner_bounds.y1 &&
                    x >= inner_bounds.x0 && x < inner_bounds.x1
                    );

            if (inside_inner) continue;
            else {
                uint32_t *cur_pixel = &bf->pixels[y * bf->width + x];
                *cur_pixel = picasso__blend_pixel(*cur_pixel, new_pixel);
            }
        }
    }
}

static inline picasso_rect picasso__make_circle_bounds(int x0, int y0, int radius)
{
    int overshoot = PICASSO_CIRCLE_DEFAULT_TOLERANCE + 1;

    picasso_rect r = {
        .x = x0 - radius - overshoot,
        .y = y0 - radius - overshoot,
        .width = (radius + overshoot) * 2 + 1,
        .height = (radius + overshoot) * 2 + 1,
    };
    return r;
}

void picasso_fill_circle(picasso_backbuffer *bf, int x0, int y0, int radius, color c)
{
    // create a box around the circle that is slightly larger then the radius
    // that is all we loop over, we clip to bounds
    picasso_draw_bounds bounds = {0};
    picasso_rect circle_box = picasso__make_circle_bounds(x0, y0, radius);
    if(!picasso__clip_rect_to_bounds(bf, &circle_box, &bounds)) return;

    uint32_t new_pixel = color_to_u32(c);

    // a^2 + b^2 = c^2
    for (int y = bounds.y0; y < bounds.y1; ++y) {
        for (int x = bounds.x0; x < bounds.x1; ++x) {
            int dx = x - x0;
            int dy = y - y0;
            if((dx*dx + dy*dy <= radius*radius + radius)){
                uint32_t *cur_pixel = &bf->pixels[y * bf->width + x];
                *cur_pixel = picasso__blend_pixel(*cur_pixel, new_pixel);
            }
        }
    }
}
void picasso_draw_circle(picasso_backbuffer *bf, int x0, int y0, int radius,int thickness, color c)
{
    picasso_draw_bounds bounds = {0};
    picasso_rect circle_box = picasso__make_circle_bounds(x0, y0, radius);
    if (!picasso__clip_rect_to_bounds(bf, &circle_box, &bounds)) return;

    uint32_t new_pixel = color_to_u32(c);

    int outer = radius * radius;
    int inner = (radius - thickness) * (radius - thickness);

    for (int y = bounds.y0; y < bounds.y1; ++y) {
        for (int x = bounds.x0; x < bounds.x1; ++x) {
            int dx = x - x0;
            int dy = y - y0;
            int dist2 = dx * dx + dy * dy;

            if (dist2 >= inner+radius && dist2 <= outer+radius) {
                uint32_t *cur_pixel = &bf->pixels[y * bf->width + x];
                *cur_pixel = picasso__blend_pixel(*cur_pixel, new_pixel);
            }
        }
    }
}
void picasso_draw_line(picasso_backbuffer *bf, int x0, int y0, int x1, int y1, color c)
{
    uint32_t new_pixel = color_to_u32(c);

    /* Bresenhams lines algorithm
     * */
    int dx = x1-x0;
    int dy = y1-y0;
    int D = 2*dy-dx;
    int y = y0;
    for(int i = x0; i < x1; ++i){
        bf->pixels[y*bf->width + i] = new_pixel;
        if (D > 0) {
            y++;
            D -= 2*dx;
        }
        D += 2*dy;
    }
}
