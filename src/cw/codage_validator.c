#include "cw.h"

static void			reg(t_cw *cw, uint8_t i_arg, uint8_t i_car, uint8_t i_op)
{
	int32_t			reg_val;

	// ft_printf(" reg ");
	cw->step += REGN_SIZE;
	if (!(cw->op[i_op].args[i_arg] & T_REG))
		cw->err = -1;
	if (cw->err)
		return ;
	cw->pos = cw->car[i_car]->pc + cw->step;
	reg_val = cw->map[PC(cw->pos)];
	if (1 <= reg_val && reg_val <= REG_NUMBER)
		cw->arg[i_arg] = reg_val - 1;
	else
		cw->err = -1;
}

static void			ind(t_cw *cw, uint8_t i_arg, uint8_t i_car, uint8_t i_op)
{
	// ft_printf(" ind ");
	cw->step += IND_SIZE;
	if (!(cw->op[i_op].args[i_arg] & T_IND))
		cw->err = -1;
	if (cw->err)
		return ;
	cw->pos = cw->car[i_car]->pc + cw->step;
	cw->arg[i_arg] = code_to_byte(cw->map, cw->pos, IND_SIZE) % IDX_MOD + cw->car[i_car]->pc;
}

static void		dir(t_cw *cw, uint8_t i_arg, uint8_t i_car, uint8_t i_op)
{
	// ft_printf(" dir ");
	cw->step += cw->op[i_op].label_size;
	if (!(cw->op[i_op].args[i_arg] & T_DIR))
		cw->err = -1;
	if (cw->err)
		return ;
	cw->arg[i_arg] = cw->car[i_car]->pc + cw->step;
}

int8_t			codage_validator(t_cw *cw, uint8_t i_car, uint8_t i_op)
{
	int8_t		i_arg;
	uint8_t		code;

	cw->step = OPC_SIZE;
	cw->cod.age = cw->map[PC(cw->car[i_car]->pc + cw->step)];
	cw->err = (cw->cod.arg.v4) ? -1 : 0;
	i_arg = -1;
	while (++i_arg < cw->op[i_op].num_args)
		if ((code = (cw->cod.age >> (6 - i_arg * 2)) & 0x3) && code == IND_CODE)
			ind(cw, i_arg, i_car, i_op);
		else if (code == REG_CODE)
			reg(cw, i_arg, i_car, i_op);
		else if (code == DIR_CODE)
			dir(cw, i_arg, i_car, i_op);
		else
			cw->err = -1;
	if (cw->flg & DEBUG)
		debug_log(cw, i_car);
	if (!cw->err)
		return (0);
	cw->car[i_car]->pc = PCV(cw->car[i_car]->pc + cw->step);
	cw->err = 0;
	return (-1);
}