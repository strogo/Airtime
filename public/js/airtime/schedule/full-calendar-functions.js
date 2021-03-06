/**
*
*	Full Calendar callback methods.
*
*/

function makeTimeStamp(date){
	var sy, sm, sd, h, m, s, timestamp;
	sy = date.getFullYear();
	sm = date.getMonth() + 1;
	sd = date.getDate();
	h = date.getHours();
	m = date.getMinutes();
	s = date.getSeconds();

	timestamp = sy+"-"+ sm +"-"+ sd +" "+ h +":"+ m +":"+ s;
	return timestamp;
}

function dayClick(date, allDay, jsEvent, view) {
	var x;
}

function eventRender(event, element, view) { 
	//element.qtip({
     //       content: event.description
     //   });

	if(view.name === 'agendaDay' || view.name === 'agendaWeek') {
		var div = $('<div/>');
		div
			.height('5px')
			.width('100px')
			.css('margin-top', '5px')
			.progressbar({
				value: event.percent
			});

		if(event.percent === 0) {
			// even at 0, the bar still seems to display a little bit of progress...
			div.find("div").hide();
		}

		$(element).find(".fc-event-title").after(div);

	}	

	if(event.backgroundColor !== "") {
		$(element)
			.css({'border-color': '#'+event.backgroundColor})
			.find(".fc-event-time, a")
				.css({'background-color': '#'+event.backgroundColor, 'border-color': '#'+event.backgroundColor});
	}
	if(event.color !== "") {
		$(element)
			.find(".fc-event-time, a")
				.css({'color': '#'+event.color});
	}
}

function eventAfterRender( event, element, view ) {
	var today = new Date();	

	if(event.isHost === true && event.start > today) {
		$(element).contextMenu(
			{menu: 'schedule_event_host_menu'}, eventMenu
		);
	}
	else{
		$(element).contextMenu(
			{menu: 'schedule_event_default_menu'}, eventMenu
		);
	}

	$(element).data({'event': event});
}

function eventClick(event, jsEvent, view) { 
	var x;
}

function eventMouseover(event, jsEvent, view) { 
}

function eventMouseout(event, jsEvent, view) { 
}

function eventDrop(event, dayDelta, minuteDelta, allDay, revertFunc, jsEvent, ui, view) {
	var url;

	if (event.repeats && dayDelta !== 0) {
		revertFunc();
		return;
	}

	url = '/Schedule/move-show/format/json';

	$.post(url, 
		{day: dayDelta, min: minuteDelta, showId: event.id},
		function(json){
			if(json.overlap) {
				revertFunc();
			}
		});
}

function eventResize( event, dayDelta, minuteDelta, revertFunc, jsEvent, ui, view ) { 
	var url;

	url = '/Schedule/resize-show/format/json';

	$.post(url, 
		{day: dayDelta, min: minuteDelta, showId: event.id},
		function(json){
			if(json.overlap) {
				revertFunc();
			}
		});
}

function getFullCalendarEvents(start, end, callback) {
	var url, start_date, end_date;

	var sy, sm, sd, ey, em, ed;
	sy = start.getFullYear();
	sm = start.getMonth() + 1;
	sd = start.getDate();

	start_date = sy +"-"+ sm +"-"+ sd;

	ey = end.getFullYear();
	em = end.getMonth() + 1;
	ed = end.getDate();
	end_date = ey +"-"+ em +"-"+ ed;

	url = '/Schedule/event-feed';
	
	if ((ed - sd) === 1) {
		url = url + '/weekday/' + start.getDay();
	}

	var d = new Date();

	$.post(url, {format: "json", start: start_date, end: end_date, cachep: d.getTime()}, function(json){
		callback(json.events);
	});
}
