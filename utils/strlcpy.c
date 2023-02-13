#include "../minishell.h"

static size_t	ft_strlen(const char *string_row)
{
	size_t	counter;

	counter = 0;
	while (*string_row != '\0')
	{
		counter = counter + 1;
		string_row = string_row + 1;
	}
	return (counter);
}

size_t	ft_strlcpy(char *dst, const char *src, size_t size)
{
	const char	*src_firstaddress;

	src_firstaddress = src;
	if (size == 0)
		return (ft_strlen(src));
	while ((size != 1) && (*src != '\0'))
	{
		*dst = (char)*src;
		dst = dst + 1;
		src = src + 1;
		size = size - 1;
	}
	*dst = '\0';
	return (ft_strlen(src_firstaddress));
}