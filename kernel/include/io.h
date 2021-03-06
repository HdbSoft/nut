#ifndef __NUT_IO_H__
#define __NUT_IO_H__

#include <nut/types.h>
#include <nut/colors.h>
#include <nut/utils.h>

#include <nut/keys.h>

#include <nut/vga.h>
#include <nut/basics.h>


u8 inb(u16 port) {
	u8 ret;

#ifdef __X86__
	asm volatile("inb %1, %0" : "=a"(ret) : "d"(port));
#else
	ret = *(volatile u8*)port;
#endif
	return ret;
}
//read from port

function outb(u16 port, u8 data) {
#ifdef __X86__
	asm volatile("outb %0, %1" : "=a"(data) : "d"(port));
#else
	*(volatile u8*)port = data;
#endif
}
//print to port

char charin() {
	char character = 0;
	while ((character = inb(KEYBOARD_PORT)) != 0) {
		if (character > 0)
			return character;
	}

	return character;
}

String strin() {
	while (charin() != ' ') {
		commands[command_num][command_char_num] = charin();
		command_char_num++;

	} if (charin() == ' ') {
		command_char_num = 0;
		command_num++;

		return commands[command_num + 1];
	}
}
//input

function newline() {
	if (next_line >= 55) {
		next_line = 0;
		clear_vga_buffer(&vga_buffer, __def_fore_color, __def_back_color);
	}

	vga_index = 80 * next_line;
	next_line++;
}

function charout(char character, u8 fore_color, u8 back_color) {
	vga_buffer[vga_index] = vga_entry(character, fore_color, back_color);
	vga_index++;
}

function strout(String string, u8 fore_color, u8 back_color) {
	u32 index = 0;

	while (string[index]) {
		charout(string[index], fore_color, back_color);
		index++;
	}
}

function gotox(u16 x) {
	vga_index += x;
}

function gotoy(u16 y) {
	vga_index = 80 * y;
}

function gotoxy(u16 x, u16 y) {
	vga_index = 80 * y;
	vga_index += x;
}
//goto functions

function centered_strout(String string, u8 fore_color, u8 back_color) {
	int str_length = strlen(string);
	if (str_length < VGA_WIDTH) {
		const int skip = (VGA_WIDTH - str_length) / 2;
		gotox(skip);
	}

	strout(string, fore_color, back_color);
	newline();
}

function intout(int num, u8 fore_color, u8 back_color) {
	char str_num[digits(num) + 1];
	ItoA(num, str_num);
	strout(str_num, fore_color, back_color);
}

function clear_screen(u8 fore_color, u8 back_color) {
	clear_vga_buffer(&vga_buffer, fore_color, back_color);
}

void puts(String s) { strout(s, actual_fore_color, actual_back_color);  }
void putc(char c)   { charout(c, actual_fore_color, actual_back_color); }
void puti(int i)    { intout(i, actual_fore_color, actual_back_color);  }

void printf(String string, ...) {
	int* var_args = &string - 4;
	char buffer[200];
	int si = 0, bi = 0, pi = 0;

	while (string[si]) {
		if (string[si] != '%') {
			buffer[bi] = string[si];
			bi++;
			si++;
		}
		else {
			int w = 0, zp = 0, pp = 0;

			si++;
			if (string[si] == '#') {
				pp = 1;
				si++;
			}
			if (string[si] == '0') {
				zp = 1;
				si++;
			}
			if (string[si] >= '1' && string[si] <= '9') {
				w = string[si] - '0';
				si++;
				if (string[si] >= '0' && string[si] <= '9') {
					w = w * 10;
					w += string[si] - '0';
					si++;
				}
			}
			if (string[si] == 's') {
				int l = strlen(var_args[pi]);
				strcpy(buffer + bi, var_args[pi]);
				bi += l;
			}
			else if (string[si] == 'd') {
				int v = var_args[pi];
				bi += __format(buffer + bi, v, w, zp, 10, 0);
			}
			else if (string[si] == 'x') {
				int v = var_args[pi];
				bi += __format(buffer + bi, v, w, zp, 16, pp);
			}
			pi--;
			si++;
		}
	}
	buffer[bi] = 0;
	puts(buffer);
}
//output

#endif //nut I/O module
