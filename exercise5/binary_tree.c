void	shift_up(t_coder *heap)
{
	while (parent > node)
	{

		parent = heap[parent_idx - 1];
		parent_idx = (current_idx - 1) / 2;
	}
	heap[parent_idx + 1] = node;
}