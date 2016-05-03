/*========== my_main.c ==========

  This is the only file you need to modify in order
  to get a working mdl project (for now).

  my_main.c will serve as the interpreter for mdl.
  When an mdl script goes through a lexer and parser, 
  the resulting operations will be in the array op[].

  Your job is to go through each entry in op and perform
  the required action from the list below:

  push: push a new origin matrix onto the origin stack
  pop: remove the top matrix on the origin stack

  move/scale/rotate: create a transformation matrix 
	based on the provided values, then 
	multiply the current top of the
	origins stack by it.

  box/sphere/torus: create a solid object based on the
	provided values. Store that in a 
	temporary matrix, multiply it by the
	current top of the origins stack, then
	call draw_polygons.

  line: create a line based on the provided values. Store 
	that in a temporary matrix, multiply it by the
	current top of the origins stack, then call draw_lines.

  save: call save_extension with the provided filename

  display: view the image live
  
  jdyrlandweaver
  =========================*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "parser.h"
#include "symtab.h"
#include "y.tab.h"

#include "misc_headers.h"
#include "matrix.h"
#include "ml6.h"
#include "display.h"
#include "draw.h"
#include "stack.h"

void my_main( int polygons ) {

  int i;
  double step;
  double xval, yval, zval;
  struct matrix *transform;
  struct matrix *tmp;
  struct stack *s;
  screen t;
  color g;

  g.red = 255;
  g.green = 0;
  g.blue = 0;
  
  s = new_stack();
  transform = new_matrix(4, 1);
  tmp = new_matrix(4, 1000);
  clear_screen( t );

  for (i=0;i<lastop;i++) {  
    switch (op[i].opcode) {
			
			// push
    case PUSH:
      push(s);
      break;

			// pop
    case POP:
      pop(s);
      break;

			// rotate
    case ROTATE:
      xval = op[i].op.rotate.axis;
      yval = op[i].op.rotate.degrees;
      yval = yval * (M_PI / 180);
      if (xval == 0) {
				tmp = make_rotX(yval);
				matrix_mult(s->data[s->top], tmp);
				copy_matrix(tmp, s->data[s->top]);
      } else if (xval == 1) {
				tmp = make_rotY(yval);
				matrix_mult(s->data[s->top], tmp);
				copy_matrix(tmp, s->data[s->top]);
      } else {
				tmp = make_rotZ(yval);
				matrix_mult(s->data[s->top], tmp);
				copy_matrix(tmp, s->data[s->top]);
      }
      break;

			// scale
    case SCALE:
      tmp = make_scale(op[i].op.scale.d[0],op[i].op.scale.d[1],
											 op[i].op.scale.d[2]);
      matrix_mult(s->data[s->top], tmp);
      copy_matrix(tmp, s->data[s->top]);
      break;

			// move
    case MOVE:
      tmp = make_translate(op[i].op.move.d[0],op[i].op.move.d[1],
													 op[i].op.move.d[2]);
      matrix_mult(s->data[s->top], tmp);
      copy_matrix(tmp, s->data[s->top]);
      break;

			// box
    case BOX:
      add_box(transform, op[i].op.box.d0[0],op[i].op.box.d0[1],
							op[i].op.box.d0[2],
							op[i].op.box.d1[0],op[i].op.box.d1[1],
							op[i].op.box.d1[2]);
      matrix_mult(s->data[s->top], transform);
      draw_polygons(transform, t, g);
      ident(transform);
      break;

			// sphere
    case SPHERE:
      add_sphere(transform,  op[i].op.sphere.d[0],op[i].op.sphere.d[1],
								 op[i].op.sphere.d[2],
								 op[i].op.sphere.r, 5);
      matrix_mult(s->data[s->top], transform);
      draw_polygons(transform, t, g);
      ident(transform);
      break;

			// torus
    case TORUS:
      add_torus(transform, op[i].op.torus.d[0],op[i].op.torus.d[1],
								op[i].op.torus.d[2],
								op[i].op.torus.r0,op[i].op.torus.r1, 5);
      matrix_mult(s->data[s->top], transform);
      draw_polygons(transform, t, g);
      ident(transform);
      break;

			// display
    case DISPLAY:
      display(t);
      break;
    }
  }
}
