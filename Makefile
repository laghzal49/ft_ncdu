all:
	@$(MAKE) -C ft_ncdu

clean:
	@$(MAKE) -C ft_ncdu clean

fclean:
	@$(MAKE) -C ft_ncdu fclean

re:
	@$(MAKE) -C ft_ncdu re

install:
	@$(MAKE) -C ft_ncdu install

uninstall:
	@$(MAKE) -C ft_ncdu uninstall

norm:
	@$(MAKE) -C ft_ncdu norm

.PHONY: all clean fclean re install uninstall norm
