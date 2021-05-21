//Chương trình vẽ 1 hình lập phương đơn vị theo mô hình lập trình OpenGL hiện đại

#include "Angel.h"  /* Angel.h là file tự phát triển (tác giả Prof. Angel), có chứa cả khai báo includes glew và freeglut*/

using namespace std;

// remember to prototype
void generateGeometry(void);
void initGPUBuffers(void);
void shaderSetup(void);
void display(void);
void keyboard(unsigned char key, int x, int y);

typedef vec4 point4;
typedef vec4 color4;

#pragma region Làm phép chiếu 

mat4 view;
GLuint view_loc;
mat4 projection;
GLuint projection_loc;

GLfloat eye_x = 2.0 , eye_y = 1.0 , eye_z = -1.5;
GLfloat l = -0.8, r = 0.8;
GLfloat bottom = -0.8, top = 0.8;
GLfloat zNear = 1.9, zFar = 4.0;
int projection_mode = 1;

//GLfloat radius = 1.5, theta = M_PI, phi = 0;
//
//const GLfloat dr = 5.0 * DegreesToRadians;

#pragma endregion 

#pragma region  Cài đặt hình lập phương đơn vị 
// Số các đỉnh của các tam giác
const int NumPoints = 36;

point4 points[NumPoints]; /* Danh sách các đỉnh của các tam giác cần vẽ*/
color4 colors[NumPoints]; /* Danh sách các màu tương ứng cho các đỉnh trên*/


point4 vertices[8]; /* Danh sách 8 đỉnh của hình lập phương*/
color4 vertex_colors[8]; /*Danh sách các màu tương ứng cho 8 đỉnh hình lập phương*/

GLuint program;
mat4 instance;
mat4 model;
GLuint model_loc;



void initCube()
{
	// Gán giá trị tọa độ vị trí cho các đỉnh của hình lập phương
	vertices[0] = point4(-0.5, -0.5, 0.5, 1.0);
	vertices[1] = point4(-0.5, 0.5, 0.5, 1.0);
	vertices[2] = point4(0.5, 0.5, 0.5, 1.0);
	vertices[3] = point4(0.5, -0.5, 0.5, 1.0);
	vertices[4] = point4(-0.5, -0.5, -0.5, 1.0);
	vertices[5] = point4(-0.5, 0.5, -0.5, 1.0);
	vertices[6] = point4(0.5, 0.5, -0.5, 1.0);
	vertices[7] = point4(0.5, -0.5, -0.5, 1.0);

	// Gán giá trị màu sắc cho các đỉnh của hình lập phương	
	vertex_colors[0] = color4(0.0, 0.0, 0.0, 1.0); // black
	vertex_colors[1] = color4(1.0, 0.0, 0.0, 1.0); // red
	vertex_colors[2] = color4(1.0, 1.0, 0.0, 1.0); // yellow
	vertex_colors[3] = color4(0.0, 1.0, 0.0, 1.0); // green
	vertex_colors[4] = color4(0.0, 0.0, 1.0, 1.0); // blue
	vertex_colors[5] = color4(1.0, 0.0, 1.0, 1.0); // magenta
	vertex_colors[6] = color4(0.0, 1.0, 1.0, 1.0); // white
	vertex_colors[7] = color4(0.0, 1.0, 1.0, 1.0); // cyan
}
int Index = 0;
void quad(int a, int b, int c, int d)  /*Tạo một mặt hình lập phương = 2 tam giác, gán màu cho mỗi đỉnh tương ứng trong mảng colors*/
{
	colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	colors[Index] = vertex_colors[a]; points[Index] = vertices[b]; Index++;
	colors[Index] = vertex_colors[a]; points[Index] = vertices[c]; Index++;
	colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	colors[Index] = vertex_colors[a]; points[Index] = vertices[c]; Index++;
	colors[Index] = vertex_colors[a]; points[Index] = vertices[d]; Index++;
}
void makeColorCube(void)  /* Sinh ra 12 tam giác: 36 đỉnh, 36 màu*/

{
	quad(1, 0, 3, 2);
	quad(2, 3, 7, 6);
	quad(3, 0, 4, 7);
	quad(6, 5, 1, 2);
	quad(4, 5, 6, 7);
	quad(5, 4, 0, 1);
}
void generateGeometry(void)
{
	initCube();
	makeColorCube();
}




void initGPUBuffers(void)
{
	// Tạo một VAO - vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Tạo và khởi tạo một buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors), NULL, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors);

}


void shaderSetup(void)
{
	// Nạp các shader và sử dụng chương trình shader
	program = InitShader("vshader1.glsl", "fshader1.glsl");   // hàm InitShader khai báo trong Angel.h
	glUseProgram(program);

	// Khởi tạo thuộc tính vị trí đỉnh từ vertex shader
	GLuint loc_vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(loc_vPosition);
	glVertexAttribPointer(loc_vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	GLuint loc_vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(loc_vColor);
	glVertexAttribPointer(loc_vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));

	model_loc = glGetUniformLocation(program, "model");
	view_loc = glGetUniformLocation(program, "view");
	projection_loc = glGetUniformLocation(program, "projection");
	
	glEnable(GL_DEPTH_TEST);

	glClearColor(1.0, 1.0, 1.0, 1.0);        /* Thiết lập màu trắng là màu xóa màn hình*/
}
#pragma endregion


#pragma region Làm nền nhà và tường

// Thiết lập kích thước 
GLfloat surface_width = 0.75, surface_height = 0.1, surface_depth = 0.75;
GLfloat wall_width = 0.75, wall_height = 0.75, wall_depth = 0.1;
GLfloat wall_theta[3] = { 0.0,0.0,0.0 };

// Vẽ nền nhà 
void surface() {
	instance =  Scale(surface_width * 2, surface_height , surface_depth * 2);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
// Vẽ 1 bức tường
void wall() {
	instance = Scale(wall_width * 2, wall_height , wall_depth) * RotateY(270);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

// Vẽ không gian bao quanh 
void makeSpace() {
	model = model * Translate(0,-wall_height* 0.5 - surface_height * 0.5,0)  * RotateX(180) ;
	surface();
	mat4 space = model;
	model = space * Translate(-surface_width + wall_depth * 0.5, -wall_height * 0.5 - surface_height * 0.5, 0) * RotateY(90);
	wall();
	model = space * Translate(0, -wall_height * 0.5 - surface_height * 0.5, -surface_depth + wall_depth * 0.5);
	wall();

}

#pragma endregion


#pragma region Làm quạt cây 


GLfloat fan_base_width = 0.45, fan_base_height = 0.1, fan_base_depth = 0.5;
GLfloat button_width = 0.05, button_height = 0.1, button_depth = 0.07;
GLfloat fan_lower_width = 0.15, fan_lower_height = 0.3, fan_lower_depth = 0.1;
GLfloat fan_upper_width = 0.15, fan_upper_height = 0.1, fan_upper_depth = 0.3;
GLfloat fan_width = 0.5, fan_height = 0.1, fan_depth = 0.025;

GLfloat fan_theta[3] = { 0.0,0.0,0.0 };
GLfloat fan_tranX = 0, fan_tranZ = 0;
GLfloat fan_scale = 1.0;
GLfloat spin_speed, spin_time;
GLfloat fan_push[4] = { 0.0,0.0,0.0,0.0 };
GLboolean isTurn = true;
GLint fan_state = 0;


void fan_base() {
	instance = Translate(0, fan_base_height * 0.5, 0) * Scale(fan_base_width, fan_base_height, fan_base_depth);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void button() {
	instance = Translate(-fan_base_width * 0.4, fan_base_height + fan_push[0], -fan_base_depth * 0.25) * Scale(button_width, button_height, button_depth) * RotateY(180);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(-fan_base_width * 0.15, fan_base_height + fan_push[1], -fan_base_depth * 0.25) * Scale(button_width, button_height, button_depth) *RotateX(-90) * RotateY(90);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(fan_base_width * 0.10, fan_base_height + fan_push[2], -fan_base_depth * 0.25) * Scale(button_width, button_height, button_depth) * RotateX(-90) * RotateY(90);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(fan_base_width * 0.35, fan_base_height + fan_push[3], -fan_base_depth * 0.25) * Scale(button_width, button_height, button_depth) * RotateX(-90) * RotateY(90);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void fan_lower() {
	instance = Translate(0, fan_lower_height * 0.5, 0) * Scale(fan_lower_width, fan_lower_height, fan_lower_depth) * RotateZ(90);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

}
void fan_upper() {
	instance = Translate(0, fan_upper_height * 0.5, 0) * Scale(fan_upper_width, fan_upper_height, fan_upper_depth) * RotateX(90) * RotateY(90);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);



}
void fan() {
	instance = Translate(0, 0, -fan_upper_depth * 0.5) * RotateZ(45) * Scale(fan_width, fan_height, fan_depth) * RotateY(180);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(0, 0, -fan_upper_depth * 0.5) * RotateZ(-45) * Scale(fan_width, fan_height, fan_depth) * RotateY(180);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}


void drawFan() {
	model = model *Translate(fan_tranX, -wall_height*0.5, fan_tranZ) * Scale(0.4, 0.4, 0.4) * RotateY(-45) ;
	fan_base();
	button();
	model = model * Translate(0, fan_base_height, 0) * Scale(1, fan_scale, 1);
	fan_lower();
	model = model * Translate(0, fan_lower_height, 0) * RotateY(fan_theta[1]) * Scale(1, 1 / fan_scale, 1);
	fan_upper();
	model = model * Translate(0, fan_upper_height * 0.5, 0) * RotateZ(fan_theta[2]*2);
	fan();
}

#pragma endregion

#pragma region Làm bộ bàn ghế
GLfloat table_surface_width = 1, table_surface_height = 0.1, table_surface_depth = 0.5;
GLfloat table_post_width = 0.05, table_post_height = 0.5, table_post_depth = 0.05;
GLfloat table_theta = 0 , chair_tranX = 0 , chair_tranZ = 0 ;
void table_surface() {
	instance =  Scale(table_surface_width, table_surface_height, table_surface_depth) * RotateZ(270);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void table_post() {
	instance =  Translate(0, -0.5 * (table_post_height + table_surface_height), 0) * Scale(table_post_width, table_post_height, table_post_depth)  * RotateZ(180);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}


void Table() {
	model = model * Translate(0,-0.25 * (table_post_height + table_surface_height),0)  *  Scale(0.5, 0.5, 0.5);
	mat4 table = model;
	table_surface();
	model = table * Translate(-0.5 * (table_surface_width - table_post_width), 0, -0.5 * (table_surface_depth - table_post_depth));
	table_post();
	model = table * Translate(0.5 * (table_surface_width - table_post_width), 0, -0.5 * (table_surface_depth - table_post_depth)) ;
	table_post();
	model = table * Translate(0.5 * (table_surface_width - table_post_width), 0, 0.5 * (table_surface_depth - table_post_depth)) ;
	table_post();
	model = table * Translate(-0.5 * (table_surface_width - table_post_width), 0, 0.5 * (table_surface_depth - table_post_depth)) ;
	table_post();
}
void Chair() {
	model = model * Translate(0, -table_post_height * 0.25-table_post_height * 0.125, 0) * Scale(0.25, 0.5, 0.5);
	Table();
}

#pragma endregion

#pragma region Làm tủ
GLfloat tv_theta[3] = { 0,0,0 };
GLfloat screen_depth = 0.6, screen_width = 0.5, screen_height = 0.5;
GLfloat shell_depth = 0.63, shell_width = 0.55, shell_height = 0.05;

GLfloat cell_cabinet_height = 0.5, cell_cabinet_width = 0.02;
GLfloat m = 0 , cabinet_tran = 0;
mat4 p;
void cell_cabinet()
{
	//trên và dưới
	instance = Translate(0, -(screen_height + shell_height) / 2, 0) * Scale(screen_width * 2, cell_cabinet_width, screen_width * 2) * RotateX(90);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(0, -(screen_height + shell_height) / 2 - cell_cabinet_height, 0) * Scale(screen_width * 2, cell_cabinet_width, screen_width * 2) * RotateX(90);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	//cạnh trái và phải 
	instance = Translate(screen_width, -(screen_height + shell_height) / 2 - (cell_cabinet_height / 2), 0) * Scale(cell_cabinet_width, cell_cabinet_height, screen_width * 2) * RotateX(90);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(-screen_width, -(screen_height + shell_height) / 2 - (cell_cabinet_height / 2), 0) * Scale(cell_cabinet_width, cell_cabinet_height, screen_width * 2) * RotateX(90);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	//ốp sau lưng
	instance = Translate(0, -(screen_height + shell_height) / 2 - (cell_cabinet_height / 2), screen_width) * Scale(screen_width * 2, cell_cabinet_height, 0.02) * RotateY(270);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);


	//ngăn đôi tủ
	instance = Translate(0, -(screen_height + shell_height) / 2 - (cell_cabinet_height) / 2, 0) * Scale(screen_width * 2, cell_cabinet_width, screen_width * 2) * RotateX(180);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	//Chắn ngăn trên
	instance = Translate(0, -((screen_height) / 2 + shell_height + (cell_cabinet_height / 4) - 0.02), -screen_width) * Scale(screen_width * 2, (cell_cabinet_height / 2) - 0.01, 0.02);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	//Núm tủ trên
	instance = Translate(0, -((screen_height) / 2 + shell_height + (0.25 * cell_cabinet_height) - 0.02), -screen_width) * Scale(0.05, 0.02, 0.1) * RotateY(90);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

}

void cell_cabinet_2()
{
	p = Translate(0, 0, -m);
	//Ngăn dưới
	instance = p * Translate(screen_width - 0.01, -((screen_height) / 2 + shell_height + (3 * cell_cabinet_height / 4) - 0.02), 0) * Scale(0.02, cell_cabinet_height / 2, screen_width * 2);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = p * Translate(-screen_width + 0.01, -((screen_height) / 2 + shell_height + (3 * cell_cabinet_height / 4) - 0.02), 0) * Scale(0.02, cell_cabinet_height / 2, screen_width * 2);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = p * Translate(0, -((screen_height) / 2 + shell_height + (3 * cell_cabinet_height / 4) - 0.02), -screen_width) * Scale(screen_width * 2, (cell_cabinet_height / 2) - 0.01, 0.02);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	//đáy ngăn bàn
	instance = p * Translate(0, -(screen_height + shell_height) / 2 - (cell_cabinet_height)+0.001, 0) * Scale(screen_width * 2, cell_cabinet_width, screen_width * 2) * RotateX(180);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	//Núm tủ
	instance = p * Translate(0, -((screen_height) / 2 + shell_height + (3 * cell_cabinet_height / 4) - 0.02), -screen_width) * Scale(0.05, 0.02, 0.1) * RotateY(90);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

}

void cabinet()
{
	cell_cabinet();
	cell_cabinet_2();
}




#pragma endregion

#pragma region Làm TV

GLfloat base_width = 0.7, base_height = 0.05, base_depth = 0.3;
GLfloat bar_width = 0.2, bar_height = 0.15, bar_depth = 0.025;
GLfloat sc_width = 0.9, sc_height = 0.6, sc_depth = 0.025;
GLfloat monitor_theta = 0;

void base() {
	instance = Translate(0, base_height * 0.5, 0) * Scale(base_width, base_height, base_depth) * RotateX(180);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void bar() {
	instance = Translate(0, bar_height * 0.5, 0) * Scale(bar_width, bar_height, bar_depth) * RotateX(90);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void screen() {
	instance = Translate(0, sc_height * 0.5, 0) * Scale(sc_width * 1.25, sc_height, sc_depth) * RotateX(-90);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
GLfloat tran = 0;

void monitor() {
	model = model * Translate(tran, 0, 0) * Translate(0, -sc_height * 0.5, 0) ;
	base();
	model = model * Translate(0, base_height, 0)  * RotateX(-15);
	bar();
	model = model * Translate(0, bar_height, (bar_depth * -cos(M_PI / 18)) - sc_depth) * RotateX(-monitor_theta) * Translate(0, -bar_height / 2, 0) * RotateX(15);
	screen();
}
#pragma endregion


void display(void)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	

	model = RotateX(-wall_theta[0]) * RotateY(wall_theta[1]);
	mat4 ctm = model;
	makeSpace();
	model = ctm * Translate(-table_surface_width * 0.4, 0, table_surface_depth);
	drawFan();
	model = ctm * Translate(table_surface_width * 0.25, 0, -surface_depth*0.25) * RotateY(table_theta);
	Table();
	model = ctm * Translate(-table_surface_width * 0.125 ,0,-table_surface_depth*0.25) *Translate(table_surface_width * 0.25, 0, -surface_depth * 0.25) * RotateY(table_theta) * Translate(chair_tranX,0,chair_tranZ);
	Chair();
	model = ctm * Translate(table_surface_width * 0.125, 0, -table_surface_depth * 0.25) * Translate(table_surface_width * 0.25, 0, -surface_depth * 0.25) * RotateY(table_theta) * Translate(-chair_tranX, 0, chair_tranZ);
	Chair();
	model = ctm * Translate(-table_surface_width * 0.125, 0, table_surface_depth * 0.25) * Translate(table_surface_width * 0.25, 0, -surface_depth * 0.25) * RotateY(table_theta) * Translate(chair_tranX, 0, -chair_tranZ);
	Chair();
	model = ctm * Translate(table_surface_width * 0.125, 0, table_surface_depth * 0.25) * Translate(table_surface_width * 0.25, 0, -surface_depth * 0.25) * RotateY(table_theta) * Translate(-chair_tranX, 0, -chair_tranZ);
	Chair();
	model = ctm * Translate(-surface_width * 0.75, 0, -surface_depth * 0.25 + cabinet_tran) * RotateY(-90) * Scale(0.5, 0.5, 0.25);
	cabinet();
	model = ctm  * Translate(-surface_width * 0.75, 0, -surface_depth * 0.25 + cabinet_tran) * RotateY(-90) * Scale(0.5, 0.5, 0.5);
	monitor();
	
	view = identity();
	if (projection_mode != 0) {
		point4 eye(eye_x, eye_y, eye_z, 1.0);
		point4 at(0.0, 0.0, 0.0, 1.0);
		vec4 up(0.0, 1.0, 0.0, 0.0);
		view = LookAt(eye, at, up);
	}
	glUniformMatrix4fv(view_loc, 1, GL_TRUE, view);

	mat4 p = identity();
	if (projection_mode == 1) {
		p = Frustum(l, r, bottom, top, zNear, zFar);
	}
	else if (projection_mode == 2) {
		p = Ortho(l - 0.5, r + 0.5, bottom - 0.5, top + 0.5, -zNear, zFar);
	}
	
	glUniformMatrix4fv(projection_loc, 1, GL_TRUE, p);

	glutSwapBuffers();
}

#pragma region Các hàm hỗ trợ chuyển động

void spin1() {

	fan_push[1] = -0.025;
	fan_theta[2] += 0.5;

	glutPostRedisplay();

}

void spin2() {

	fan_push[2] = -0.025;
	if (fan_theta[2] > 360) {
		fan_theta[2] -= 360;
	}
	fan_theta[2] += 1.0;
	glutPostRedisplay();

}

void spin3() {

	fan_push[3] = -0.025;
	fan_theta[2] += 1.5;

	glutPostRedisplay();

}
void spin_off() {
	fan_push[0] = -0.025;
	fan_theta[2] += spin_speed;
	if (spin_speed >= 0) {
		spin_speed -= 0.00025;

	}
	if (spin_speed < spin_time * 0.9) {
		fan_push[0] = 0.0025;
	}

	glutPostRedisplay();

}
void ZoomIn() {
	zNear += 0.1;
	l += 0.1;
	r -= 0.1;
	bottom += 0.1;
	top -= 0.1;
	zFar += 0.1;
}

void ZoomOut() {
	zNear -= 0.1;
	l -= 0.1;
	r += 0.1;
	bottom -= 0.1;
	top += 0.1;
	zFar += 0.1;
}

void Reset() {
	wall_theta[0] = 0;
	wall_theta[1] = 0;
	fan_theta[0] = 0;
	fan_theta[1] = 0;
	fan_theta[2] = 0;
	fan_tranX = 0, fan_tranZ = 0;
	fan_scale = 1.0;
	if (isTurn == true) {
			isTurn = false;
			fan_push[fan_state] = 0.0025;
			if (fan_state == 1) {
				spin_time = spin_speed = 0.5;
			}
			else if (fan_state == 2) {
				spin_time = spin_speed = 1.0;
			}
			else if (fan_state == 3) {
				spin_time = spin_speed = 1.5;
			}
			glutIdleFunc(spin_off);
	}
	table_theta = 0, chair_tranX = 0, chair_tranZ = 0;
	cabinet_tran = 0;
	m = 0;
	monitor_theta = 0;
	eye_x = 2.0, eye_y = 1.0, eye_z = -1.5;
	l = -0.8, r = 0.8;
	bottom = -0.8, top = 0.8;
	zNear = 1.9, zFar = 4.0;
	projection_mode = 1;
}

#pragma endregion

/* Kịch bản chuyển động
* - Mô hình :
a-A : xoay mô hình theo trục x
s-S : xoay mô hình theo trục y

- Quạt :
0 : tắt quạt từ từ 
1 : bật số 1
2 : bật số 2
3 : bật số 3
v-V , b-B : thay đổi vị trí quạt ( dịch chuyển đế )
n-N : di chuyển thân dưới quạt
m-M : di chuyển thân trên quạt

- Kệ và tv :
j - J : tịnh tiến bàn 
k - K : mở ngăn kéo
l - L : gập TV

- Bàn : 
f-F : xoay bàn
g-G , h - H : tịnh tiến ghế

- Phép chiếu : 
x-X , y-Y , z-Z : tịnh tiến Camera theo các trục x,y,z

- phím spacebar : khởi động lại

- Chuột trái : chiếu phối cảnh
- Nút giữa   : tọa độ thế giới thực  
- Chuột phải : chiếu song song

*/
void keyboard(unsigned char key, int x, int y)
{
	// keyboard handler

	switch (key) {
	case 033:			// 033 is Escape key octal value
		exit(1);		// quit program
		break;


#pragma region  Chuyển động quạt

	case 'v':
		if (fan_tranX <= 1.0) {
			fan_tranX += 0.05;
		}
		break;
	case 'V':
		if (fan_tranX >= -0.1) {
			fan_tranX -= 0.05;
		}
		break;
	case 'b':
		if (fan_tranZ >= -1.0) {
			fan_tranZ -= 0.05;
		}
		break;
	case 'B':
		if (fan_tranZ <= 0.05) {
			fan_tranZ += 0.05;
		}
		break;
	case 'n':
		if (fan_scale < 2.0) {
			fan_scale += 0.1;
		}

		break;
	case 'N':
		if (fan_scale > 1.0) {
			fan_scale -= 0.1;
		}
		break;
	case 'm':
		fan_theta[1] += 5;
		break;
	case 'M':
		fan_theta[1] -= 5;
		break;


	case '0':
		if (isTurn == true) {
			isTurn = false;
			fan_push[fan_state] = 0.0025;
			if (fan_state == 1) {
				spin_time = spin_speed = 0.5;
			}
			else if (fan_state == 2) {
				spin_time = spin_speed = 1.0;
			}
			else if (fan_state == 3) {
				spin_time = spin_speed = 1.5;
			}
			glutIdleFunc(spin_off);

		}
		break;
	case '1':
		if (isTurn == false) {
			fan_push[0] = 0.0025;
		}
		isTurn = true;
		fan_push[fan_state] = 0.0025;
		fan_state = 1;
		glutIdleFunc(spin1);
		break;
	case '2':
		if (isTurn == false) {
			fan_push[0] = 0.0025;
		}
		isTurn = true;
		fan_push[fan_state] = 0.0025;
		fan_state = 2;
		glutIdleFunc(spin2);
		break;
	case '3':
		if (isTurn == false) {
			fan_push[0] = 0.0025;
		}
		isTurn = true;
		fan_push[fan_state] = 0.0025;
		fan_state = 3;
		glutIdleFunc(spin3);
		break;

#pragma endregion

#pragma region Chuyển động kệ và TV
	case 'j':
		if (cabinet_tran < 0.3) {
			cabinet_tran += 0.1;
		}
		break;
	case 'J':
		if (cabinet_tran > -0.3) {
			cabinet_tran -= 0.1;
		}
		break;
	case 'k':
		m += 0.2;
		if (m > screen_width * 1.5)
			m = screen_width * 1.5;

		break;
	case 'K':
		m -= 0.2;
		if (m < 0)
			m = 0;

		break;
	case 'l':
		if (monitor_theta < 15) {
			monitor_theta += 5;
		}
		break;
	case 'L':
		if (monitor_theta > -15) {
			monitor_theta -= 5;
		}
		break;
	
#pragma endregion

#pragma region Chuyển động của bàn
	case 'f':
		table_theta += 5;
		break;
	case 'F':
		table_theta -= 5;
		break;
	case 'g':
		if (chair_tranX <= 0.03) {
			chair_tranX += 0.01;
		}
		break;
	case 'G':
		if (chair_tranX > -0.03) {
			chair_tranX -= 0.01;
		}
	case 'H':
		if (chair_tranZ <= 0.03) {
			chair_tranZ += 0.01;
		}
		break;
	case 'h':
		if (chair_tranZ > -0.03) {
			chair_tranZ -= 0.01;
		}
		break;
#pragma endregion

#pragma region Chuyển động mô hình
	case 's':
		wall_theta[1] += 5;
		break;
	case 'S':
		wall_theta[1] -= 5;
		break;
	case 'a':
		wall_theta[0] += 5;

		break;
	case 'A':
		wall_theta[0] -= 5;
		break;
#pragma endregion

#pragma region Thay đổi góc chiếu
	case 'x':
		eye_x -= 0.2;
		zNear -= 0.2;
		zFar += 0.2;
		break;
	case 'X':
		eye_x += 0.2;
		zNear += 0.2;
		zFar += 0.2;
		break;
	case 'y':
		eye_y -= 0.1;
		ZoomIn();
		break;
	case 'Y':
		eye_y += 0.1;
		ZoomOut();
		break;
	case 'Z':
		eye_z += 0.1;
		ZoomOut();
		break;
	case 'z':
		eye_z -= 0.1;
		ZoomIn();
		break;
	case ' ':
		Reset();
		break;

#pragma endregion

	}
	glutPostRedisplay();
}
void mouse(int button, int state, int x, int y)
{

	if (state == GLUT_DOWN) {
		switch (button) {
		case GLUT_LEFT_BUTTON: projection_mode = 2; break;
		case GLUT_MIDDLE_BUTTON: projection_mode = 0; break;
		case GLUT_RIGHT_BUTTON: projection_mode = 1; break;
		}
	}
}


void reshape(int width, int height)
{
	glViewport(0, 0, width, height);
}


#pragma endregion

int main(int argc, char** argv)
{
	// main function: program starts here

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(640, 640);
	glutInitWindowPosition(100, 150);
	glutCreateWindow("Drawing a Cube");


	glewInit();

	generateGeometry();
	initGPUBuffers();
	shaderSetup();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutReshapeFunc(reshape);
	glutMainLoop();
	return 0;
}
