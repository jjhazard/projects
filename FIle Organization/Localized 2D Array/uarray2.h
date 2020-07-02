#ifndef Uarray2_INCLUDED
#define Uarray2_INCLUDED
#define T UArray2_T
typedef struct T *T;
extern T UArray2_new (int width, int height, int size);
extern void UArray2_free  (T *array2);
extern int UArray2_width (T array2);
extern int UArray2_height(T array2);
extern int UArray2_size (T array2);
extern int UArray2_blocksize (T array2);
extern void *UArray2_at(T array2, int i, int j);
extern void UArray2_map_row_major(T array2,
	        void apply(int i, int j, T array2, void *elem, void *cl),
				  void *cl);
extern void UArray2_map_col_major(T array2,
	        void apply(int i, int j, T array2, void *elem, void *cl),
				  void *cl);
extern void UArray2_map(T array2,
	     void apply(int i, int j, T array2, void *elem, void *cl),
			void *cl);
#undef T
#endif
