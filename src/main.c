#include <stdio.h>
#include "../include/allocator.h"

#define MAX_NAME 50

typedef struct Student {
        char name[MAX_NAME + 1];
        float gpa;
} Student;

int main() {
        memory_init();

        const int student_count = 5;
        Student **students = allocate(sizeof(Student *) * student_count);

        for (int st = 1; st <= student_count; st++) {
                Student *student = allocate(sizeof(Student));

                printf("Enter name for student %d: ", st);
                scanf("%51[^\n]", student->name);
                printf("Enter gpa for student %d: ", st);
                scanf("%f", &student->gpa);
                while (getchar() != '\n');

                students[st - 1] = student;
        }

        draw_memory();
        release(students[3]);
        draw_memory();
        release(students[4]);
        draw_memory();
        release(students[0]);
        release(students[1]);
        release(students[2]);
        draw_memory();

        return 0;
}
