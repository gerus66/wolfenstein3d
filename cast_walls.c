/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cast_walls.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bturcott <bturcott@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/03/20 20:42:36 by mbartole          #+#    #+#             */
/*   Updated: 2019/03/27 16:09:53 by mbartole         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "wolf.h"

static int		first_shift_x(float ang, int bound, int x, int y)
{
	if (QT_12(ang))
		bound++;
	if (QT_14(ang))
		return (x + (y - bound * BLOCK) / tan(ang));
	else
		return (x - (y - bound * BLOCK) / tan(M_PI - ang));
}

static int		first_shift_y(float ang, int bound, int x, int y)
{
	if (QT_23(ang))
	{
		bound++;
		ang = ang - M_PI;
	}
	if (QT_12(ang))
		return (y - (bound * BLOCK - x) * tan(ang));
	else
		return (y + (x - bound * BLOCK) * tan(ang));
}

static int		cur_column(int x, float ang) //TODO fix corners
{
//	if (x % BLOCK)
		return (x / BLOCK);
//	if (QT_23(ang))
//		return (x / BLOCK - 1);
//	return (x / BLOCK + 1);
}

static int		cur_row(int y, float ang) //TODO fix corners
{
//	if (y % BLOCK)
		return (y / BLOCK);
//	if (QT_12(ang))
//		return (y / BLOCK - 1);
//	return (y / BLOCK + 1);
}

static float	get_dist_x(t_sdl *sbox, float ang, int *fl, int *x)
{
	int	next_r;
	int	cur_c;
	int xa;

	next_r = (QT_12(ang)) ? sbox->cam.y / BLOCK - 1 : sbox->cam.y / BLOCK + 1;
	xa = ABS(BLOCK / tan((QT_14(ang)) ? ang : M_PI - ang));
	*x = first_shift_x(ang, next_r, sbox->cam.x, sbox->cam.y);
	cur_c = cur_column(*x, ang);
	while (1)
	{
		if (cur_c >= MAP_W(sbox->map) || *x < 0 ||
				next_r >= MAP_H(sbox->map) || next_r <= -1)
		{
			*fl = 0;
//printf("S");
//			printf("s[%d %d] ", *x, next_r);
			return (sqrt(pow(sbox->cam.y - 
				(QT_12(ang) ? next_r + 1 : next_r) * BLOCK, 2) + 
				pow(sbox->cam.x - *x, 2)));
		}
		if (((t_point *)sbox->map->cont)[sbox->map->offset * next_r + cur_c].h)
		{
			*fl = 1;
//			printf("X");
		//	printf("x[%d %d] ", *x, next_r);
			return (sqrt(pow(sbox->cam.y -
					(QT_12(ang) ? next_r + 1 : next_r) * BLOCK, 2) +
						pow(sbox->cam.x - *x, 2)));
		}
		next_r = (QT_12(ang)) ? next_r - 1 : next_r + 1;
		*x = (QT_14(ang)) ? *x + xa : *x - xa;
		cur_c = cur_column(*x, ang);
	}
	*fl = 0;
	return (0.0);
}

static float	get_dist_y(t_sdl *sbox, float ang, int *fl, int *y)
{
	int	cur_r;
	int	next_c;
	int ya;

	*fl = 1;	
	next_c = (QT_14(ang)) ? sbox->cam.x / BLOCK + 1 : sbox->cam.x / BLOCK - 1;
	ya = ABS(BLOCK * tan((QT_14(ang)) ? ang : M_PI - ang));
	*y = first_shift_y(ang, next_c, sbox->cam.x, sbox->cam.y);
	cur_r = cur_row(*y, ang);
//	printf("fsY %d nextC %d ", y, next_c);
	while (1)
	{
		if (cur_r >= MAP_H(sbox->map) || *y < 0 || 
				next_c >= MAP_W(sbox->map) || next_c <= -1)
		{
			*fl = 0;
	//		printf("S");
//			printf("s[%d %d] ", *x, next_r);
			return (sqrt(pow(sbox->cam.x -
					(QT_23(ang) ? next_c + 1 : next_c) * BLOCK, 2) +
					 pow(sbox->cam.y - *y, 2)));
		}
		if (((t_point *)sbox->map->cont)[sbox->map->offset * cur_r + next_c].h)
		{
			*fl = 1;
//			printf("Y");
		//	printf("y[%d %d] ", next_c, *y);
			return (sqrt(pow(sbox->cam.x -
					(QT_23(ang) ? next_c + 1 : next_c) * BLOCK, 2) +
					 pow(sbox->cam.y - *y, 2)));
		}
		next_c = QT_14(ang) ? next_c + 1 : next_c - 1;
		*y = QT_12(ang) ? *y - ya : *y + ya;
		cur_r = cur_row(*y, ang);
	}
	*fl = 0;
	return (0.0);
}

# define N 0x42F4AD
# define E 0x8A6DA3
# define W 0xBD76F7
# define S 0xF44262
# define SKY 0xA3F8FF
# define FLOUR 0x9E9380

static int      just_color(float ang, int h, int j, char fl)
{
	if (j >= WIN_H / 2 + h)
		return (FLOUR);
	if (j < WIN_H / 2 - h || (fl == 's' && j < WIN_H / 2 + h))
		return (SKY);
	if ((fl == 'x' && QT_12(ang)))
		return (N);
	if ((fl == 'x' && QT_34(ang)))
		return (S);
	if ((fl == 'y' && QT_23(ang)))
		return (W);
	if ((fl == 'y' && QT_14(ang)))
		return (E);
	return (0xFFFFFF);
}

void	cast_walls(t_sdl *sbox, unsigned int *map)
{
	int	i;
	int	j;
	float dist;
	float ydist;
	int h;
	float ang;
	char	fl;
	int		offset, yoffset;

	printf("map %d x %d \n", MAP_W(sbox->map), MAP_H(sbox->map));
	printf("I m in [%d %d] looking at %d grad ", sbox->cam.x / BLOCK,
			sbox->cam.y / BLOCK, (int)(sbox->cam.angle / M_PI * 180));
	printf("(from %d to %d grad)\n", 
			(int)((sbox->cam.angle - FOV / 2) / M_PI * 180),
			(int)((sbox->cam.angle + FOV / 2) / M_PI * 180));
	ang = sbox->cam.angle + FOV / 2;
	i = -1;
	while (++i < WIN_W)
	{
		if (ang > M_PI)
			ang = -(2 * M_PI - ang);
		else if (ang < -M_PI)
			ang = 2 * M_PI + ang;
		int flx, fly;
		dist = get_dist_x(sbox, ang, &flx, &offset);
		ydist = get_dist_y(sbox, ang, &fly, &yoffset);
		if (!flx && !fly)
		{
			fl = 's';
			if (ydist < dist)
			{
				//		printf("Y!\n");
				dist = ydist;
				// offset = yoffset;
			}
		}
		else
		{
		//	printf("<%.1f VS %.1f > ", dist, ydist);
			fl = 'x';
			if ((fly && ydist < dist) || !flx)
			{
				dist = ydist;
				offset = yoffset;
				fl = 'y';
			}
		}
		dist *= cos(ang - sbox->cam.angle);
		h = WALL_H * DIST / dist;
		//printf("/%d %c/ ", offset % BLOCK, fl);
		if (sbox->flags[1])
		{
		//	printf("angle %f h = %d x = %d offset = %d %c\n", ang, h, i, offset, fl);
			paint_walls(sbox, ang, (int[]){h, i, offset % BLOCK}, fl);
		}
		else
		{
			j = -1;
			while (++j < WIN_H)
				map[i + j * WIN_W] = just_color(ang, h, j, fl);
		}
		ang -= STEP;
	}
}
