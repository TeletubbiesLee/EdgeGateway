struct2json库的修改记录-Lei.L

(注：后续对struct2json库的修改必须在此文件中添加修改日期和修改者，记录格式保持统一)
记录格式：
[日期] [修改人]
[修改文件]
	1.[修改概括]
		[具体修改代码]
	2.[修改概括]
		[具体修改代码]
	...

[日期] [修改人]
[修改文件]
	...
(注：不同日期间用空行分隔，不同文件间用空行分隔)


2020/04/30 Lei.L
cJSON.c
	1.大约109-129行，将计算数字的解析程序进行修改，原作者的程序执行一直出错：
		//n=sign*n*pow(10.0,(scale+subscale*signsubscale));	/* number = +/- number.fraction * 10^+/- exponent */
		n = sign * n;
		int i = 0, temp = 0;
		if((scale + subscale * signsubscale) >= 0)
		{
			temp = scale+subscale * signsubscale;
			for(i = 0; i < temp; i++)
			{
				n *= 10.0;
			}
		}
		else if((scale + subscale * signsubscale) < 0)
		{
			temp = -(scale+subscale * signsubscale);
			for(i = 0; i < temp; i++)
			{
				n /= 10.0;
			}
		}




