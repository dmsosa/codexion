void	print_exit(const char *str)
{
	printf("%s", str);
	exit(1);
}

void	print_exit_usage(void)
{
	printf("Usage: codexion [-b bonus] arg...\n");
	exit(1);
}
