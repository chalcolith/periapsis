//
// $Id$
//
// Copyright (c) 2008, The Periapsis Project. All rights reserved. 
// 
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are 
// met: 
// 
// * Redistributions of source code must retain the above copyright notice, 
//   this list of conditions and the following disclaimer. 
// 
// * Redistributions in binary form must reproduce the above copyright 
//   notice, this list of conditions and the following disclaimer in the 
//   documentation and/or other materials provided with the distribution. 
// 
// * Neither the name of the The Periapsis Project nor the names of its 
//   contributors may be used to endorse or promote products derived from 
//   this software without specific prior written permission. 
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS 
// IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED 
// TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A 
// PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER 
// OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

#include "platform/display.hpp"
#include "platform/lowlevel.hpp"
#include "platform/extensions.hpp"

#include <cmath>

namespace gsgl
{

    using namespace data;
    using namespace math;

    namespace platform
    {

        config_variable<int> display::DISPLAY_WIDTH(L"display/width", 1028);
        config_variable<int> display::DISPLAY_HEIGHT(L"display/height", 768);

        int display::current_fbo_id = 0;

    
        //////////////////////////////////////////

        display::display(const int & width, const int & height, bool is_console) 
            : surface(0), is_console(is_console), opengl_fbo_id(0)
        {
            unsigned int rmask, gmask, bmask, amask;
            
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
            rmask = 0xff000000;
            gmask = 0x00ff0000;
            bmask = 0x0000ff00;
            amask = 0x000000ff;
#else
            
            rmask = 0x000000ff;
            gmask = 0x0000ff00;
            bmask = 0x00ff0000;
            amask = 0xff000000;
#endif
            
            if (is_console)
            {
                // create the window
                SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
                SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 0);
                
                surface = SDL_SetVideoMode(width, height, 32, SDL_OPENGL);

                // center the window
                center_console_window(width, height);
            }
            else
            {
                surface = SDL_CreateRGBSurface(SDL_HWSURFACE | SDL_SRCALPHA, width, height, 32, rmask, gmask, bmask, amask);
            }

            // set up framebuffer if able
            if (surface)
            {
                gsgl::platform::init_extensions();

                /// \todo check for framebuffer extension, set up fbo
            }
        } // display::display()

        
        display::~display()
        {
            unbind();

            if (!is_console)
                SDL_FreeSurface(surface);
        } // display::~display()

        
        int display::get_width() const
        {
            return surface->w;
        } // display::get_width()

        
        int display::get_height() const
        {
            return surface->h;
        } // display::get_height()


        gsgl::real_t display::get_aspect_ratio() const
        {
            return static_cast<gsgl::real_t>(surface->w) / static_cast<gsgl::real_t>(surface->h);
        } // display::get_aspect_ratio()


        void display::bind()
        {
            if (current_fbo_id == opengl_fbo_id)
            {
                return;
            }

            if (GL_EXT_framebuffer_object)
            {
                if (is_console)
                    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
                else
                    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, opengl_fbo_id);
            }
        } // display::bind()


        void display::unbind()
        {
            if (GL_EXT_framebuffer_object)
            {
                glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
            }
        } // display::unbind()


        void display::define_ambient_light(const color & ambient)
        {
            bind();

            glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient.ptr());                                CHECK_GL_ERRORS();
        } // display::define_ambient_light()
        

        int display::get_max_lights()
        {
            bind();

            int max_lights;
            glGetIntegerv(GL_MAX_LIGHTS, &max_lights);                                                              CHECK_GL_ERRORS();
            return max_lights;
        } // display::get_max_lights()


        void display::define_light(const int & light_number, const math::transform & modelview, const math::vector & position,
                                   const color & ambient, const color & diffuse, const color & specular,
                                   const gsgl::real_t & attenuation_constant, const gsgl::real_t & attenuation_linear, const gsgl::real_t & attenuation_quadratic)
        {
            bind();

            glMatrixMode(GL_MODELVIEW);
            glLoadMatrixf(modelview.ptr());

            int gl_light = GL_LIGHT0 + light_number;

            glEnable(gl_light);
            glLightfv(gl_light, GL_AMBIENT, ambient.ptr());
            glLightfv(gl_light, GL_DIFFUSE, diffuse.ptr());
            glLightfv(gl_light, GL_SPECULAR, specular.ptr());
            glLightfv(gl_light, GL_POSITION, position.ptr());

            glLightf(gl_light, GL_CONSTANT_ATTENUATION, attenuation_constant);
            glLightf(gl_light, GL_LINEAR_ATTENUATION, attenuation_linear);
            glLightf(gl_light, GL_QUADRATIC_ATTENUATION, attenuation_quadratic);
        } // display::define_light()


        void display::clear(const gsgl::flags_t flags, const color & clear_color)
        {
            unsigned int gl_flags = 0;
            
            if (flags & CLEAR_COLOR)
            {
                gl_flags |= GL_COLOR_BUFFER_BIT;
                glClearColor(clear_color[color::COMPONENT_RED], clear_color[color::COMPONENT_GREEN], clear_color[color::COMPONENT_BLUE], clear_color[color::COMPONENT_ALPHA]);
            }

            if (flags & CLEAR_DEPTH)
            {
                gl_flags |= GL_DEPTH_BUFFER_BIT;
            }

            glClear(gl_flags);
        } // display::clear()


        void display::draw_point(const vector & v, const gsgl::real_t & width)
        {
            glPointSize(width);
            glBegin(GL_POINTS);
            glVertex3f(v.get_x(), v.get_y(), v.get_z());
            glEnd();
            glPointSize(1.0f);
        } // display::draw_point()


        void display::draw_line_2d(float x1, float y1, float x2, float y2, float width)
        {
            glBegin(GL_LINES);
            glVertex2f(x1, y1);
            glVertex2f(x2, y2);
            glEnd();                                                                                                    CHECK_GL_ERRORS();
        } // display::draw_line_2d()


        void display::draw_line_2d(int x1, int y1, int x2, int y2, int width)
        {
            draw_line_2d(static_cast<float>(x1),
                         static_cast<float>(y1),
                         static_cast<float>(x2),
                         static_cast<float>(y2));
        } // display::draw_line_2d()


        void display::draw_rect_2d(float x1, float y1, float x2, float y2,
                                   float s1, float t1, float s2, float t2)
        {
#if 0
            glBegin(GL_QUADS);

            // lower left
            glNormal3f(0, 0, 1);
            glTexCoord2f(s1, t1);
            glVertex2f(x1, y1);

            // lower right
            glNormal3f(0, 0, 1);
            glTexCoord2f(s2, t1);
            glVertex2f(x2, y1);

            // upper right
            glNormal3f(0, 0, 1);
            glTexCoord2f(s2, t2);
            glVertex2f(x2, y2);

            // upper left
            glNormal3f(0, 0, 1);
            glTexCoord2f(s1, t2);
            glVertex2f(x1, y2);

            glEnd();

            CHECK_GL_ERRORS();

#else
            glPolygonMode(GL_FRONT, GL_FILL);
            glBegin(GL_TRIANGLE_STRIP);

            // upper left
            //glNormal3f(0, 0, 1);
            glTexCoord2f(s1, t2);
            glVertex2f(x1, y2);

            // lower left
            //glNormal3f(0, 0, 1);
            glTexCoord2f(s1, t1);
            glVertex2f(x1, y1);

            // upper right
            //glNormal3f(0, 0, 1);
            glTexCoord2f(s2, t2);
            glVertex2f(x2, y2);

            // lower right
            //glNormal3f(0, 0, 1);
            glTexCoord2f(s2, t1);
            glVertex2f(x2, y1);

            glEnd();                                                                                                CHECK_GL_ERRORS();
#endif
        } // display::draw_rectangle()


#ifdef WIN32

        void display::center_console_window(const int & width, const int & height)
        {
            SDL_SysWMinfo info;
            SDL_VERSION(&info.version);

            if (SDL_GetWMInfo(&info))
            {
                HWND desktop = GetDesktopWindow();
                RECT rect;
                if (desktop && GetWindowRect(desktop, &rect))
                {
                    int left = ((rect.right - rect.left) < 2*(rect.bottom - rect.top)) ? (rect.left + rect.right - width)/2 : (3*rect.left + rect.right - 2*width)/4;
                    int top = (rect.top + rect.bottom - height)/2;

                    SetWindowPos(info.window, HWND_TOP, left, top, width, height, SWP_SHOWWINDOW);
                }
            }
        } // display::center_console_window()

#else
#error You must define display::center_console_window() for your platform!
#endif

        //////////////////////////////////////////////////////////////

        display::scoped_viewport::scoped_viewport(display & parent)
            : parent(parent)
        {
            parent.bind();
            glViewport(0, 0, parent.get_width(), parent.get_height());
        } // display::scoped_viewport::scoped_viewport()


        display::scoped_viewport::~scoped_viewport()
        {
        } // display::scoped_viewport::~scoped_viewport()


        //////////////////////////////////////////

        display::scoped_state::scoped_state(display & parent, const gsgl::flags_t & flags)
            : parent(parent)
        {
            parent.bind();

            glPushAttrib(GL_ALL_ATTRIB_BITS);                                                                       CHECK_GL_ERRORS();
            glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);                                                          CHECK_GL_ERRORS();

            enable(flags);
        } // display::scoped_state::scoped_state()


        display::scoped_state::~scoped_state()
        {
            glPopClientAttrib();
            glPopAttrib();

            parent.unbind();
        } // display::scoped_state::~scoped_state()


        void display::scoped_state::enable(const gsgl::flags_t & flags)
        {
            if (flags & ENABLE_DEPTH)
            {
                glEnable(GL_DEPTH_TEST);                                                                                CHECK_GL_ERRORS();
            }
            else
            {
                glDisable(GL_DEPTH_TEST);
            }

            if (flags & ENABLE_BLEND)
            {
                glEnable(GL_BLEND);                                                                                     CHECK_GL_ERRORS();
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);                                                      CHECK_GL_ERRORS();
            }
            else
            {
                glDisable(GL_BLEND);
            }

            if (flags & ENABLE_ANTIALIAS)
            {
                glEnable(GL_POINT_SMOOTH);                                                                              CHECK_GL_ERRORS();
                glEnable(GL_LINE_SMOOTH);                                                                               CHECK_GL_ERRORS();
                //glEnable(GL_POLYGON_SMOOTH);                                                                            CHECK_GL_ERRORS();
            }
            else
            {
                glDisable(GL_POINT_SMOOTH);
                glDisable(GL_LINE_SMOOTH);
            }

            if (flags & ENABLE_SMOOTH_SHADING)
            {
                glShadeModel(GL_SMOOTH);                                                                                CHECK_GL_ERRORS();
            }
            else
            {
                glShadeModel(GL_FLAT);                                                                                  CHECK_GL_ERRORS();
            }

            if (flags & ENABLE_FILLED_POLYS)
            {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);                                                              CHECK_GL_ERRORS();
            }
            else
            {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);                                                              CHECK_GL_ERRORS();
            }

            if (flags & ENABLE_BF_CULL)
            {
                glEnable(GL_CULL_FACE);                                                                                 CHECK_GL_ERRORS();
            }
            else
            {
                glDisable(GL_CULL_FACE);
            }

            if (flags & ENABLE_TEXTURES)
            {
                glEnable(GL_TEXTURE_2D);                                                                                CHECK_GL_ERRORS();
            }
            else
            {
                glDisable(GL_TEXTURE_2D);                                                                               CHECK_GL_ERRORS();
            }

            if (flags & ENABLE_LIGHTING)
            {
                glEnable(GL_LIGHTING);                                                                                  CHECK_GL_ERRORS();
                glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);                                                    CHECK_GL_ERRORS();
                glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);                                                       CHECK_GL_ERRORS();
            }
            else
            {
                glDisable(GL_LIGHTING);                                                                                 CHECK_GL_ERRORS();
            }

            if (flags & ENABLE_BUFFERS)
            {
                glEnableClientState(GL_VERTEX_ARRAY);                                                                   CHECK_GL_ERRORS();
                glEnableClientState(GL_NORMAL_ARRAY);                                                                   CHECK_GL_ERRORS();
                glEnableClientState(GL_TEXTURE_COORD_ARRAY);                                                            CHECK_GL_ERRORS();
                glEnableClientState(GL_INDEX_ARRAY);                                                                        CHECK_GL_ERRORS();
            }
            else
            {
                glDisableClientState(GL_VERTEX_ARRAY);                                                                   CHECK_GL_ERRORS();
                glDisableClientState(GL_NORMAL_ARRAY);                                                                   CHECK_GL_ERRORS();
                glDisableClientState(GL_TEXTURE_COORD_ARRAY);                                                            CHECK_GL_ERRORS();
                glDisableClientState(GL_INDEX_ARRAY);                                                                        CHECK_GL_ERRORS();
            }
        } // display::scoped_state::enable()


        //////////////////////////////////////////

        display::scoped_modelview::scoped_modelview(display & parent, const transform *mv)
            : parent(parent), mv(mv)
        {
            parent.bind();

            glMatrixMode(GL_MODELVIEW);                                                                                 CHECK_GL_ERRORS();
            glPushMatrix();                                                                                             CHECK_GL_ERRORS();

            if (mv) 
            {
                glLoadMatrixf(mv->ptr());                                                                               CHECK_GL_ERRORS();
            }
        } // display::scoped_modelview::scoped_modelview()


        display::scoped_modelview::~scoped_modelview()
        {
            parent.bind();

            glMatrixMode(GL_MODELVIEW);
            glPopMatrix();
        } // display::scoped_modelview::~scoped_modelview()


        void display::scoped_modelview::translate(const vector & v)
        {
            parent.bind();
            glTranslatef(v.get_x(), v.get_y(), v.get_z());
        } // display::scoped_modelview::translate()


        void display::scoped_modelview::translate(const gsgl::real_t & x, const gsgl::real_t & y, const gsgl::real_t & z)
        {
            parent.bind();
            glTranslatef(x, y, z);
        } // display::scoped_modelview::translate()


        void display::scoped_modelview::scale(const vector & v)
        {
            parent.bind();
            glScalef(v.get_x(), v.get_y(), v.get_z());
        } // display::scoped_modelview::scale()


        void display::scoped_modelview::scale(const gsgl::real_t & x, const gsgl::real_t & y, const gsgl::real_t & z)
        {
            parent.bind();
            glScalef(x, y, z);
        } // display::scoped_modelview::scale()


        void display::scoped_modelview::mult(const transform & t)
        {
            parent.bind();
            glMultMatrixf(t.ptr());
        } // display::scoped_modelview::mult()


        display::scoped_perspective::scoped_perspective(display & parent, const gsgl::real_t & field_of_view, const gsgl::real_t & aspect_ratio, const gsgl::real_t & near_distance, const gsgl::real_t & far_distance)
            : parent(parent)
        {
            parent.bind();

            glMatrixMode(GL_PROJECTION);                                                                        CHECK_GL_ERRORS();
            glPushMatrix();                                                                                     CHECK_GL_ERRORS();
            glLoadIdentity();                                                                                   CHECK_GL_ERRORS();

            gluPerspective(field_of_view, aspect_ratio, near_distance, far_distance);                           CHECK_GL_ERRORS();
        } // display::scoped_perspective::scoped_perspective()


        display::scoped_perspective::~scoped_perspective()
        {
            parent.bind();

            glMatrixMode(GL_PROJECTION);
            glPopMatrix();
        } // display::scoped_perspective::~scoped_perspective()


        display::scoped_ortho::scoped_ortho(display & parent)
            : parent(parent)
        {
            parent.bind();

            glMatrixMode(GL_PROJECTION);
            glPushMatrix();                                                                                     CHECK_GL_ERRORS();
            glLoadIdentity();                                                                                   CHECK_GL_ERRORS();
            
            glOrtho(0, static_cast<float>(parent.get_width()), 0, static_cast<float>(parent.get_height()), -1, 1);
        } // display::scoped_orth::scoped_orth()


        display::scoped_ortho::~scoped_ortho()
        {
            parent.bind();

            glMatrixMode(GL_PROJECTION);                                                                        CHECK_GL_ERRORS();
            glPopMatrix();                                                                                      CHECK_GL_ERRORS();
        } // display::scoped_ortho::~scoped_ortho()


        //////////////////////////////////////////

        display::scoped_buffer::scoped_buffer(display & parent, const primitive_type & pt, vertex_buffer & vb)
            : parent(parent), vertices(&vb), normals(0), texcoords(0), indices(0), gl_type(get_gl_type(pt)), interleaved(false), interleaved_start(0)
        {
            init();
        } // display::scoped_buffer::scoped_buffer()


        display::scoped_buffer::scoped_buffer(display & parent, const primitive_type & pt, vertex_buffer & vb, index_buffer & ib, bool interleaved, int interleaved_start)
            : parent(parent), vertices(&vb), normals(0), texcoords(0), indices(&ib), gl_type(get_gl_type(pt)), interleaved(interleaved), interleaved_start(interleaved_start)
        {
            init();
        } // display::scoped_buffer::scoped_buffer()


        display::scoped_buffer::scoped_buffer(display & parent, const primitive_type & pt, vertex_buffer & vb, vertex_buffer & nb, vertex_buffer & tc)
            : parent(parent), vertices(&vb), normals(&nb), texcoords(&tc), indices(0), gl_type(get_gl_type(pt)), interleaved(false)
        {
            init();
        } // display::scoped_buffer::scoped_buffer()


        display::scoped_buffer::scoped_buffer(display & parent, const primitive_type & pt, vertex_buffer & vb, vertex_buffer & nb, vertex_buffer & tc, index_buffer & ib)
            : parent(parent), vertices(&vb), normals(&nb), texcoords(&tc), indices(&ib), gl_type(get_gl_type(pt)), interleaved(false)
        {
            init();
        } // display::scoped_buffer::scoped_buffer()


        display::scoped_buffer::~scoped_buffer()
        {
            if (vertices) vertices->unbind();
            if (normals) normals->unbind();
            if (texcoords) texcoords->unbind();
            if (indices) indices->unbind();
        } // display::scoped_buffer::~scoped_buffer()


        void display::scoped_buffer::init()
        {
            if (vertices && interleaved)
            {
                vertices->bind();                                                                                       CHECK_GL_ERRORS();
                glInterleavedArrays(GL_T2F_N3F_V3F, 0, vbuffer::VBO_OFFSET<vbuffer::index_t>(interleaved_start));       CHECK_GL_ERRORS();
            }
            else
            {
                if (vertices)
                {
                    vertices->bind();                                                                                       CHECK_GL_ERRORS();
                    glVertexPointer(3, GL_FLOAT, 0, 0);                                                                     CHECK_GL_ERRORS();
                }

                if (normals)
                {
                    normals->bind();                                                                                        CHECK_GL_ERRORS();
                    glNormalPointer(GL_FLOAT, 0, 0);                                                                        CHECK_GL_ERRORS();
                }

                if (texcoords)
                {
                    texcoords->bind();                                                                                      CHECK_GL_ERRORS();
                    glTexCoordPointer(2, GL_FLOAT, 0, 0);                                                                   CHECK_GL_ERRORS();
                }
            }

            if (indices)
            {
                indices->bind();                                                                                            CHECK_GL_ERRORS();
            }
        } // display::scoped_buffer::init()


        void display::scoped_buffer::draw(int count, int start)
        {
            if (indices)
            {
                glDrawElements(gl_type, count, GL_UNSIGNED_INT, vbuffer::VBO_OFFSET<vbuffer::index_t>(start));              CHECK_GL_ERRORS();
            }
            else
            {
                glDrawArrays(gl_type, start, count);                                                                        CHECK_GL_ERRORS();
            }
        } // display::scoped_buffer::draw()


        int display::scoped_buffer::get_gl_type(const primitive_type & pt)
        {
            switch (pt)
            {
            case PRIMITIVE_POINTS:
                return GL_POINTS;
            case PRIMITIVE_LINES:
                return GL_LINES;
            case PRIMITIVE_TRIANGLES:
                return GL_TRIANGLES;
            case PRIMITIVE_TRIANGLE_STRIP:
                return GL_TRIANGLE_STRIP;
            case PRIMITIVE_TRIANGLE_FAN:
                return GL_TRIANGLE_FAN;
            default:
                throw internal_exception(__FILE__, __LINE__, L"Unknown primitive type.");
            }
        } // display::scoped_buffer::get_gl_type()


        //////////////////////////////////////////

        display::scoped_text::scoped_text(display & parent)
            : parent(parent)
        {
            parent.bind();

            // record 3d info
            glGetIntegerv(GL_VIEWPORT, text_draw_viewport);                                                         CHECK_GL_ERRORS();
            glGetFloatv(GL_MODELVIEW_MATRIX, text_draw_modelview.ptr());                                            CHECK_GL_ERRORS();
            glGetFloatv(GL_PROJECTION_MATRIX, text_draw_projection.ptr());                                          CHECK_GL_ERRORS();
            text_draw_pm = text_draw_projection * text_draw_modelview;                                              CHECK_GL_ERRORS();

            // set up projection and modelview for drawing text
            glPushAttrib(GL_ALL_ATTRIB_BITS);                                                                       CHECK_GL_ERRORS();
            glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);                                                          CHECK_GL_ERRORS();
            
            glMatrixMode(GL_PROJECTION);                                                                            CHECK_GL_ERRORS();
            glPushMatrix();                                                                                         CHECK_GL_ERRORS();
            glLoadIdentity();                                                                                       CHECK_GL_ERRORS();
            glOrtho(0.0, parent.surface->w, 0.0, parent.surface->h, -1, 1);                                         CHECK_GL_ERRORS();

            glMatrixMode(GL_MODELVIEW);                                                                             CHECK_GL_ERRORS();
            glPushMatrix();                                                                                         CHECK_GL_ERRORS();
            glLoadIdentity();                                                                                       CHECK_GL_ERRORS();
        } // display::scoped_text::scoped_text()


        display::scoped_text::~scoped_text()
        {
            parent.bind();

            // we don't user CHECK_GL_ERRORS here, as no good ever comes from throwing in a destructor
            glMatrixMode(GL_MODELVIEW);
            glPopMatrix();

            glMatrixMode(GL_PROJECTION);
            glPopMatrix();

            glPopClientAttrib();
            glPopAttrib();
        } // display::scoped_text::~scoped_text()


        void display::scoped_text::draw_2d(const gsgl::real_t & x, const gsgl::real_t & y, const font *f, const gsgl::string & str)
        {
            assert(f);

            parent.bind();

            glMatrixMode(GL_MODELVIEW);                                                                             CHECK_GL_ERRORS();
            glLoadIdentity();                                                                                       CHECK_GL_ERRORS();
            glTranslatef(x, y, 0);                                                                                  CHECK_GL_ERRORS();
            f->draw(str);
            
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
        } // dispaly::scoped_text::draw_2d()


        void display::scoped_text::draw_3d(const math::vector & p, const font *f, const gsgl::string & str, const gsgl::real_t & x_offset, const gsgl::real_t & y_offset)
        {
            parent.bind();

            vector p_in_clip_space = text_draw_pm * p;
            
            if (p_in_clip_space.get_z() >= 0)
            {
                gsgl::real_t wx = text_draw_viewport[0] + text_draw_viewport[2]*(p_in_clip_space.get_x() + 1)/2;
                gsgl::real_t wy = text_draw_viewport[1] + text_draw_viewport[3]*(p_in_clip_space.get_y() + 1)/2;

                if (wx >= -100 && wx < parent.surface->w && wy >= -10 && wy < parent.surface->h+10)
                    draw_2d(wx + x_offset, wy + y_offset, f, str);
            }
        } // display::scoped_text::draw_3d()


    } // namespace platform
    
} // namespace gsgl
